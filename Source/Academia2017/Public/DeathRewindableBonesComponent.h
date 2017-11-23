#pragma once

#include "Components/ActorComponent.h"
#include "BoneSnapshotInfo.h"
#include "TimeEventManagerComponent.h"
#include "DeathRewindManagerComponent.h"
#include "Runtime/Engine/Classes/Components/PoseableMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h"
#include "DeathRewindableBonesComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACADEMIA2017_API UDeathRewindableBonesComponent : public UActorComponent {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Time, meta = (AllowPrivateAccess = "true"))
	bool rewinding = false;

	UPROPERTY(EditAnywhere)
	UMaterialInterface *InvisibleMaterial;

public:
	UDeathRewindableBonesComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = Record)
	bool IsRewinding();

private:
	UTimeEventManagerComponent *timeEventManager = nullptr;;
	UDeathRewindManagerComponent *rewindManager = nullptr;
	UPoseableMeshComponent *poseableMesh = nullptr;
	USkeletalMeshComponent *mesh = nullptr;
	TArray<FBoneSnapshotInfo> rewindStack;
	TArray<UMaterialInstanceDynamic *> dynamicMaterials;

	void Rewind(const TArray<FBoneSnapshotInfo> &snapshotStack);
	void StopRewind();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StopRewind();
	
	void SetMeshVisibility(bool visible);
	void CacheDynamicMaterials();
};
