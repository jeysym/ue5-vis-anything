#pragma once

#include "CoreMinimal.h"
#include "VisAnythingComponent.generated.h"

UENUM(BlueprintType)
enum class EVisVectorSpace : uint8
{
	ActorSpace,
	ComponentSpace,
	WorldSpace,
};

USTRUCT(BlueprintType)
struct FPropertyVisConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHidden = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor VisColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVisVectorSpace VisSpace = EVisVectorSpace::ActorSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector VisOffset = FVector::Zero();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowOnlySingleArrayItem = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition="bShowOnlySingleArrayItem", ClampMin="0", ClampMax="10"))
	int32 ArrayIndex = 0;
};

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VISANYTHING_API UVisAnythingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVisAnythingComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDisplayFullPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHideZeroVectors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReflectActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bReflectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UActorComponent> ComponentClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FPropertyVisConfig> PropertyConfigs;

	TArray<const UObject*> GetReflectedObjects() const;

	const FPropertyVisConfig& GetConfigForProperty(const FString& PropName) const;
};
