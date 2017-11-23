#pragma once

#include "Components/ActorComponent.h"
#include "DeathSnapshotInfo.h"
#include "TimeEventManagerComponent.h"
#include "DamageableCharacterComponent.h"
#include "DeathRewindManagerComponent.h"
#include "DeathRecordableComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UDeathRecordableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDeathRecordableComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void StartRecording();
	void StopRecording();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StartRecording();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StopRecording();

	UFUNCTION(BlueprintPure, Category = Record)
	bool IsRecording();

private:
	TArray<FDeathSnapshotInfo> rewindStack;
	UTimeEventManagerComponent *timeEventManager = nullptr;
	UDamageableCharacterComponent *damageableComponent = nullptr;
	UDeathRewindManagerComponent *rewindManager = nullptr;
};
