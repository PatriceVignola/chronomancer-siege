#pragma once

#include "Components/ActorComponent.h"
#include "RecordInterupterComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API URecordInterupterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URecordInterupterComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	UInputComponent *inputComponent;

	void RequestRecordInterupt();
};
