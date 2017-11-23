#pragma once

#include "Components/ActorComponent.h"
#include "SnapshotInfo.h"
#include "TimeEventManagerComponent.h"
#include "AttackHitbox.h"
#include "DamageableCharacterComponent.h"
#include "RecordableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnableRecordPostProcessRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisableRecordPostProcessRequested);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACADEMIA2017_API URecordableComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Curve)
	TSubclassOf<AActor> RewindCheckpointClass;

public:	
	URecordableComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void StartRecording(bool isFixedDuration = false, float duration = 0.f);
	void StopRecording();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StartRecording();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StopRecording();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_CancelRecording();

	UFUNCTION(BlueprintPure, Category = Record)
	bool IsRecording() { return isRecording; }

	UPROPERTY(BlueprintAssignable, Category = Record)
	FEnableRecordPostProcessRequested OnEnableRecordPostProcessRequested;

	UPROPERTY(BlueprintAssignable, Category = Record)
	FDisableRecordPostProcessRequested OnDisableRecordPostProcessRequested;

private:
	TArray<FSnapshotInfo> rewindStack;
	UTimeEventManagerComponent *timeEventManager = nullptr;
	UAttackHitbox *attackHitBox = nullptr;
	UDamageableCharacterComponent *damageableComponent = nullptr;
	URewindManagerComponent *rewindManager = nullptr;
	AActor *rewindCheckpoint = nullptr;
	float relativeTime = 0.f;
	bool isRecording = false;
	bool destroyAfterRewind = false;

	void DestroyRewindCheckpoint(const TArray<FSnapshotInfo> &stack);
	void CancelRecording();
};
