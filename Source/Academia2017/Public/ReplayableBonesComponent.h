#pragma once

#include "Components/ActorComponent.h"
#include "BoneSnapshotInfo.h"
#include "Components/SceneComponent.h"
#include "Runtime/Engine/Classes/Components/PoseableMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h"
#include "ReplayableBonesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloneAttackStarted, int32, AttackType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloneLeftFootstepPlayed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloneRightFootstepPlayed);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UReplayableBonesComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Curve)
	FName SocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Hitbox")
	USceneComponent *attackHitbox = nullptr;
public:	
	UReplayableBonesComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	void CacheInitialRecordStack(const TArray<FBoneSnapshotInfo> &recordStack);

	UPROPERTY(BlueprintAssignable, Category = Attack)
	FOnCloneAttackStarted OnCloneAttackStarted;

	UPROPERTY(BlueprintAssignable, Category = Footstep)
	FOnCloneLeftFootstepPlayed OnCloneLeftFootstepPlayed;

	UPROPERTY(BlueprintAssignable, Category = Footstep)
	FOnCloneRightFootstepPlayed OnCloneRightFootstepPlayed;

private:
	TArray<FBoneSnapshotInfo> replayStack, rewindStack;
	UTimeEventManagerComponent *timeEventManager = nullptr;
	UPoseableMeshComponent *poseableMesh = nullptr;
	USkeletalMeshComponent *mesh = nullptr;

	bool recording = false, replaying = false;
	float timeSinceReplayStarted = 0.f;
	int stackIndex = 0;

	void CacheNewTimeZero(bool isFixedDuration, float duration);
	void NoticeRecordStopped();
	void StartReplayingBones(const TArray<FBoneSnapshotInfo> &snapshotData);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_CacheNewTimeZero();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_NoticeRecordStopped();
};
