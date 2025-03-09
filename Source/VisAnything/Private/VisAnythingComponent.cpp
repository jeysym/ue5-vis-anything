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
		Result.Add(GetOwner());
	}

	if (bReflectComponent && ComponentClass.Get())
	{
		const UObject* Component = GetOwner()->FindComponentByClass(ComponentClass);
		Result.Add(Component);
	}

	return Result;
}

const FPropertyVisConfig& UVisAnythingComponent::GetConfigForProperty(const FString& PropName) const
{
	static FPropertyVisConfig DefaultConfig;

	const FPropertyVisConfig* Config = PropertyConfigs.Find(PropName);
	return (Config) ? *Config : DefaultConfig;
}
