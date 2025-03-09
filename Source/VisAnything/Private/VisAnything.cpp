// Copyright Epic Games, Inc. All Rights Reserved.

#include "VisAnything.h"
#include "VisAnythingVisualizer.h"
#include "VisAnythingComponent.h"
#include "UObject/UnrealTypePrivate.h"

#if WITH_EDITOR
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#endif

#define LOCTEXT_NAMESPACE "FVisAnythingModule"

void FVisAnythingModule::StartupModule()
{
#if WITH_EDITOR
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	if (GUnrealEd)
	{
		TSharedPtr<FVisAnythingVisualizer> Visualizer = MakeShareable(new FVisAnythingVisualizer);
		GUnrealEd->RegisterComponentVisualizer(UVisAnythingComponent::StaticClass()->GetFName(), Visualizer);
		Visualizer->OnRegister();
	}
#endif
}

void FVisAnythingModule::ShutdownModule()
{
#if WITH_EDITOR
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(UVisAnythingComponent::StaticClass()->GetFName());
	}
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVisAnythingModule, VisAnything)

static FString GetPropertyName(const FProperty* Prop)
{
#if WITH_EDITOR
	return Prop->GetDisplayNameText().ToString();
#else
	return Prop->GetName();
#endif
}

static void CrawlProperty(TArray<FReflectedVector>& Out, const FProperty* Prop, const void* ContainerPtr, FString Path, TArray<FArrayAccess> ArrayMetadata);

static void CrawlStruct(TArray<FReflectedVector>& Out, const FStructProperty* Prop, const void* ContainerPtr, FString Path, TArray<FArrayAccess> ArrayMetadata)
{
	const void* StructPtr = Prop->ContainerPtrToValuePtr<const void*>(ContainerPtr);
	const UScriptStruct* StructStruct = Prop->Struct;

	if (Prop->GetCPPType(nullptr, 0) == TEXT("FVector"))
	{
		FReflectedVector& NewReflectedVector = Out.AddDefaulted_GetRef();
		NewReflectedVector.Name = GetPropertyName(Prop);
		NewReflectedVector.FullName = Path;
		NewReflectedVector.Value = (FVector*)StructPtr;
		NewReflectedVector.ArrayMetadata = ArrayMetadata;
		return;
	}

	for (TFieldIterator<FProperty> It(StructStruct); It; ++It)
	{
		FString Path2 = FString::Printf(TEXT("%s.%s"), *Path, *GetPropertyName(*It));
		CrawlProperty(Out, *It, StructPtr, Path2, ArrayMetadata);
	}
}

static void CrawlArray(TArray<FReflectedVector>& Out, const FArrayProperty* Prop, const void* ContainerPtr, FString Path, TArray<FArrayAccess> ArrayMetadata)
{
	const void* ArrayPtr = Prop->ContainerPtrToValuePtr<const void*>(ContainerPtr);
	FScriptArrayHelper Helper(Prop, ArrayPtr);

	for (int32 Idx = 0; Idx < Helper.Num(); ++Idx)
	{
		TArray<FArrayAccess> ArrayMetadata2 = ArrayMetadata;
		ArrayMetadata2.Emplace(GetPropertyName(Prop->Inner), Idx);
		CrawlProperty(Out, Prop->Inner, Helper.GetRawPtr(Idx), FString::Printf(TEXT("%s[%d]"), *Path, Idx), ArrayMetadata2);
	}
}

static void CrawlProperty(TArray<FReflectedVector>& Out, const FProperty* Prop, const void* ContainerPtr, FString Path, TArray<FArrayAccess> ArrayMetadata)
{
	if (const FStructProperty* StructProp = CastField<FStructProperty>(Prop))
	{
		CrawlStruct(Out, StructProp, ContainerPtr, Path, ArrayMetadata);
	}

	if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Prop))
	{
		CrawlArray(Out, ArrayProp, ContainerPtr, Path, ArrayMetadata);
	}
}

TArray<FReflectedVector> ReflectAllVectorFields(const UObject* Obj)
{
	TArray<FReflectedVector> Result;

	for (TFieldIterator<FProperty> FieldIt(Obj->GetClass()); FieldIt; ++FieldIt) {
		CrawlProperty(Result, *FieldIt, (const void*)Obj, GetPropertyName(*FieldIt), {});
	}

	return Result;
}
