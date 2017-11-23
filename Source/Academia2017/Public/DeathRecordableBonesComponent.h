#pragma once

#include "Components/ActorComponent.h"
#include "TimeEventManagerComponent.h"
#include "AttackHitbox.h"
#include "DeathRewindManagerComponent.h"
#include "Runtime/Engine/Classes/Components/PoseableMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h"
#include "DeathRecordableBonesComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UDeathRecordableBonesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDeathRecordableBonesComponent();
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
	TArray<FBoneSnapshotInfo> rewindStack;
	UTimeEventManagerComponent *timeEventManager = nullptr;
	UAttackHitbox *attackHitBox = nullptr;
	UDeathRewindManagerComponent *rewindManager = nullptr;
	USkeletalMeshComponent *mesh = nullptr;
};
