#include "VisAnythingVisualizer.h"
#include "VisAnythingComponent.h"
#include "VisAnything.h"

#if WITH_EDITOR
#include "CanvasTypes.h"
#include "CanvasItem.h"

void FVisAnythingVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	const UVisAnythingComponent* Comp = Cast<UVisAnythingComponent>(Component);
	if (!Comp)
		return;

	const AActor* Owner = Comp->GetOwner();

	TArray<const UObject*> Objs = Comp->GetReflectedObjects();
	for (const UObject* Obj : Objs)
	{
		auto RefVecs = ReflectAllVectorFields(Obj);

		for (auto& RefVec : RefVecs)
		{
			const FPropertyVisConfig& VisConfig = Comp->GetConfigForProperty(RefVec.Name);

			if (VisConfig.bHidden || (Comp->bHideZeroVectors&& RefVec.Value->IsNearlyZero()))
				continue;

			bool bSkip = false;
			for (const FArrayAccess& ArrayAccess : RefVec.ArrayMetadata)
			{
				if (Comp->GetConfigForProperty(ArrayAccess.ArrayPropName).bShowOnlySingleArrayItem)
				{
					if (Comp->GetConfigForProperty(ArrayAccess.ArrayPropName).ArrayIndex != ArrayAccess.Index)
						bSkip = true;
				}
			}
			if (bSkip)
				continue;

			FVector WorldPos = *RefVec.Value;
			switch (VisConfig.VisSpace)
			{
			case EVisVectorSpace::WorldSpace:
				WorldPos = *RefVec.Value;
				break;
			case EVisVectorSpace::ActorSpace:
				WorldPos = Owner->GetTransform().TransformPosition(*RefVec.Value);
				break;
			case EVisVectorSpace::ComponentSpace:
				if (const USceneComponent* SceneComp = Cast<USceneComponent>(Comp))
				{
					WorldPos = SceneComp->GetComponentTransform().TransformPosition(*RefVec.Value);
				}
				break;
			}

			WorldPos += VisConfig.VisOffset;

			PDI->DrawPoint(WorldPos, VisConfig.VisColor, 5.0f, SDPG_Foreground);
		}
	}
}

void FVisAnythingVisualizer::DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	const UVisAnythingComponent* Comp = Cast<UVisAnythingComponent>(Component);
	if (!Comp)
		return;

	const AActor* Owner = Comp->GetOwner();

	TArray<const UObject*> Objs = Comp->GetReflectedObjects();
	for (const UObject* Obj : Objs)
	{
		auto RefVecs = ReflectAllVectorFields(Obj);

		struct FVectorGroup
		{
			FVector WorldPos = FVector::Zero();
			TArray<FReflectedVector> RefVecs;
		};

		TArray<FVectorGroup> VecGroups;

		for (auto& RefVec : RefVecs)
		{
			const FPropertyVisConfig& VisConfig = Comp->GetConfigForProperty(RefVec.Name);

			if (VisConfig.bHidden || (Comp->bHideZeroVectors && RefVec.Value->IsNearlyZero()))
				continue;

			bool bSkip = false;
			for (const FArrayAccess& ArrayAccess : RefVec.ArrayMetadata)
			{
				if (Comp->GetConfigForProperty(ArrayAccess.ArrayPropName).bShowOnlySingleArrayItem)
				{
					if (Comp->GetConfigForProperty(ArrayAccess.ArrayPropName).ArrayIndex != ArrayAccess.Index)
						bSkip = true;
				}
			}
			if (bSkip)
				continue;

			FVector WorldPos = *RefVec.Value;
			switch (VisConfig.VisSpace)
			{
			case EVisVectorSpace::WorldSpace:
				WorldPos = *RefVec.Value;
				break;
			case EVisVectorSpace::ActorSpace:
				WorldPos = Owner->GetTransform().TransformPosition(*RefVec.Value);
				break;
			case EVisVectorSpace::ComponentSpace:
				if (const USceneComponent* SceneComp = Cast<USceneComponent>(Comp))
				{
					WorldPos = SceneComp->GetComponentTransform().TransformPosition(*RefVec.Value);
				}
				break;
			}

			WorldPos += VisConfig.VisOffset;

			bool bGroupFound = false;
			for (auto& VecGroup : VecGroups)
			{
				if (VecGroup.WorldPos.Equals(WorldPos))
				{
					VecGroup.RefVecs.Add(RefVec);
					bGroupFound = true;
				}
			}

			if (!bGroupFound)
			{
				FVectorGroup NewGroup;
				NewGroup.WorldPos = WorldPos;
				NewGroup.RefVecs.Add(RefVec);
				VecGroups.Add(NewGroup);
			}
		}

		for (const auto& VecGroup : VecGroups)
		{
			FVector2D PixelPos;
			View->WorldToPixel(VecGroup.WorldPos, PixelPos);

			FString Text;

			if (VecGroup.RefVecs.Num() == 1)
			{
				Text += Comp->bDisplayFullPath ? RefVecs[1].FullName : RefVecs[1].Name;
			}
			else
			{
				for (const auto& RefVec : VecGroup.RefVecs)
				{
					Text += TEXT("+ ");
					Text += Comp->bDisplayFullPath ? RefVec.FullName : RefVec.Name;
					Text += TEXT("\n");
				}
			}

			FCanvasTextItem TextItem(
				PixelPos,
				FText::FromString(Text),
				GEngine->GetMediumFont(),
				FLinearColor::Black);
			Canvas->DrawItem(TextItem);
		}
	}
}
#endif