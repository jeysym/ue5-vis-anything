#pragma once

#include "CoreMinimal.h"
#include "ComponentVisualizer.h"

class FVisAnythingVisualizer : public FComponentVisualizer
{

public:
	void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) override;
};
