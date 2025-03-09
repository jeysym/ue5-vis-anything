// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FVisAnythingModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

struct FArrayAccess
{
	FString ArrayPropName;
	int32 Index = 0;
};

struct FReflectedVector
{
	FString FullName;
	FString Name;
	FVector* Value;
	TArray<FArrayAccess> ArrayMetadata;
};

TArray<FReflectedVector> ReflectAllVectorFields(const UObject* Obj);