// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "SnapshotInfo.h"
#include "TimeEventManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "ClonableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UClonableComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Clone, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> CloneClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Clone, meta = (AllowPrivateAccess = "true"))
	TArray<UMaterialInterface *> CloneMaterials;

public:	
	// Sets default values for this component's properties
	UClonableComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	void CloneOwner(const TArray<FSnapshotInfo> &snapshotData);
	void CacheBoneStack(const TArray<FBoneSnapshotInfo> &snapshotData);
	UTimeEventManagerComponent *timeEventManager;

	TArray<FSnapshotInfo> replayData;
	TArray<FBoneSnapshotInfo> cachedBoneStack;
	bool isSavingReplayData = false;

	UPROPERTY(ReplicatedUsing = OnCloneReplicated)
	AActor *replicatedClone;

	UFUNCTION()
	void OnCloneReplicated();
};
