#pragma once

#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Components/PoseableMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h"
#include "TimeEventManagerComponent.h"
#include "BoneSnapshotInfo.h"
#include "RewindManagerComponent.h"
#include "RecordableBonesComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API URecordableBonesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URecordableBonesComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, Category = Recording)
	void SaveLeftFootstepPlayed();

	UFUNCTION(BlueprintCallable, Category = Recording)
	void SaveRightFootstepPlayed();

private:
	USkeletalMeshComponent *mesh = nullptr;
	UTimeEventManagerComponent *timeEventManager;
	TArray<FBoneSnapshotInfo> recordStack, replayStack;
	URewindManagerComponent *rewindManager = nullptr;
	float relativeTime = 0.0f;
	bool isRecording = false;
	bool frameLeftFootstep = false;
	bool frameRightFootstep = false;
	int frameAttackType = -1;

	void StartRecordingBones(bool isFixedDuration, float duration);
	void StopRecordingBones();
	void CancelRecordingBones();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StartRecordingBones();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StopRecordingBones();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_CancelRecordingBones();

	void RecordAttackSound(int attackType);
};
