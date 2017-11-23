#pragma once

#include "Components/ActorComponent.h"
#include "TimeEventManagerComponent.h"
#include "Runtime/Engine/Classes/Components/PoseableMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h"
#include "BoneSnapshotInfo.h"
#include "CloneComponent.h"
#include "RewindManagerComponent.h"
#include "WarriorCharacter.h"
#include "RewindableBonesComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API URewindableBonesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URewindableBonesComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(EditAnywhere)
	UMaterialInterface *InvisibleMaterial;

private:
	TArray<FBoneSnapshotInfo> rewindStack, replayStack;
	TArray<UMaterialInstanceDynamic *> dynamicMaterials;
	USkeletalMeshComponent *mesh = nullptr;
	UTimeEventManagerComponent *timeEventManager;
	UPoseableMeshComponent *poseableMesh = nullptr;
	URewindManagerComponent *rewindManager = nullptr;
	bool pendingSetMeshHidden = false;

	void StartRecordingBones();
	void StartRewindingBones(const TArray<FBoneSnapshotInfo> &recordStack);
	void StopRewindingBones();
	void CancelRewindingBones();
	bool isRewinding = false;
	float relativeTime = 0.0f;

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StopRewindingBones();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_CancelRewindingBones();

	void SetMeshVisibility(bool visible);
	void CacheDynamicMaterials();
};
