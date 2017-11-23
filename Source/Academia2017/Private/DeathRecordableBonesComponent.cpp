#include "Academia2017.h"
#include "DeathRecordableBonesComponent.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "EventManager.h"

UDeathRecordableBonesComponent::UDeathRecordableBonesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDeathRecordableBonesComponent::BeginPlay()
{
	Super::BeginPlay();

	if(UActorComponent *comp = GetOwner()->FindComponentByClass(UTimeEventManagerComponent::StaticClass()))
	{
		timeEventManager = Cast<UTimeEventManagerComponent>(comp);
	}

	mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	rewindManager = GetWorld()->GetGameState()->FindComponentByClass<UDeathRewindManagerComponent>();

	if(GetOwner()->HasAuthority())
	{
		EventManager::OnCheckpointReached.AddUObject(this, &UDeathRecordableBonesComponent::StartRecording);
		EventManager::OnDeathRewindFinished.AddUObject(this, &UDeathRecordableBonesComponent::StartRecording);
		EventManager::OnGameOver.AddUObject(this, &UDeathRecordableBonesComponent::StopRecording);
	}
}

void UDeathRecordableBonesComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if(IsRecording())
	{
		if (mesh)
		{
			rewindStack.Add(FBoneSnapshotInfo(mesh->GetComponentSpaceTransforms(), rewindManager->GetRelativeTime(), -1, false, false));
		}
	}
}

void UDeathRecordableBonesComponent::StartRecording()
{
	if(GetOwner()->HasAuthority())
	{
		RPC_StartRecording();
	}
}

void UDeathRecordableBonesComponent::RPC_StartRecording_Implementation()
{
	rewindStack.Empty();
}

void UDeathRecordableBonesComponent::StopRecording()
{
	if(GetOwner()->HasAuthority())
	{
		RPC_StopRecording();
	}
}

void UDeathRecordableBonesComponent::RPC_StopRecording_Implementation()
{
	if(timeEventManager)
	{
		timeEventManager->OnDeathBoneRecordStopped.Broadcast(rewindStack);
	}
}

bool UDeathRecordableBonesComponent::IsRecording()
{
	return rewindManager ? rewindManager->IsRecording() : false;
}