// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "CloneComponent.h"
#include "ClonableComponent.h"
#include "RecorderComponent.h"
#include "RecordableComponent.h"
#include "ReplayableBonesComponent.h"
#include "Net/UnrealNetwork.h"
#include "DisappearAfterRewindComponent.h"

UClonableComponent::UClonableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UClonableComponent::BeginPlay()
{
	Super::BeginPlay();

	if(UActorComponent *comp = GetOwner()->FindComponentByClass(UTimeEventManagerComponent::StaticClass()))
	{
		timeEventManager = Cast<UTimeEventManagerComponent>(comp);
		timeEventManager->OnRewindFinished.AddUObject(this, &UClonableComponent::CloneOwner);
		timeEventManager->OnBoneRecordStopped.AddUObject(this, &UClonableComponent::CacheBoneStack);
	}
}

void UClonableComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UClonableComponent, replicatedClone);
}

void UClonableComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UClonableComponent::CloneOwner(const TArray<FSnapshotInfo> &snapshotData)
{
	if (GetOwner()->HasAuthority())
	{
		const FTransform ownerTransform = GetOwner()->GetTransform();

		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		replicatedClone = GetWorld()->SpawnActor(CloneClass, &ownerTransform, spawnParams);

		OnCloneReplicated();

		if (replicatedClone)
		{
			UCloneComponent *cloneComponent = NewObject<UCloneComponent>(replicatedClone);
			cloneComponent->RegisterComponent();
			cloneComponent->Replay(snapshotData);
		}
	}
}

void UClonableComponent::CacheBoneStack(const TArray<FBoneSnapshotInfo> &snapshotData)
{
	cachedBoneStack = snapshotData;
}

void UClonableComponent::OnCloneReplicated()
{
	if (replicatedClone)
	{
		if (UActorComponent *comp = replicatedClone->GetComponentByClass(UReplayableBonesComponent::StaticClass()))
		{
			Cast<UReplayableBonesComponent>(comp)->CacheInitialRecordStack(cachedBoneStack);
		}

		cachedBoneStack.Empty();
	}
}