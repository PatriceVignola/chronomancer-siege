#pragma once

#include "Components/ActorComponent.h"
#include "MenuSpawnerComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UMenuSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMenuSpawnerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
};
