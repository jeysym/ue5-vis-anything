#include "VisAnythingComponent.h"

UVisAnythingComponent::UVisAnythingComponent()
{
	bDisplayFullPath = true;
	bHideZeroVectors = true;
	bReflectActor = true;
	bReflectComponent = false;
}

TArray<const UObject*> UVisAnythingComponent::GetReflectedObjects() const
{
	TArray<const UObject*> Result;

	if (bReflectActor)
	{
		const AActor* Owner = GetOwner();
		check(Owner);
		Result.Add(Owner);
	}

	if (bReflectComponent && ComponentClass.Get())
	{
		const UObject* Component = GetOwner()->FindComponentByClass(ComponentClass);
		if (Component)
			Result.Add(Component);
	}

	return Result;
}

const FPropertyVisConfig& UVisAnythingComponent::GetConfigForProperty(const FString& PropName) const
{
	const FPropertyVisConfig* Config = PropertyConfigs.Find(PropName);
	return (Config) ? *Config : DefaultPropertyConfig;
}
