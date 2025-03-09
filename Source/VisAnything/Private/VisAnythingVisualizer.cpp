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

			FVector2D PixelPos;
			View->WorldToPixel(WorldPos, PixelPos);

			FCanvasTextItem TextItem(
				PixelPos,
				FText::FromString(Comp->bDisplayFullPath ? RefVec.FullName : RefVec.Name),
				GEngine->GetMediumFont(),
				FLinearColor::Black);
			Canvas->DrawItem(TextItem);
		}
	}
}
#endif