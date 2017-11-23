#include "Academia2017.h"
#include "RecordableComponent.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "EventManager.h"

URecordableComponent::URecordableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void URecordableComponent::BeginPlay()
{
	Super::BeginPlay();

	if(UActorComponent *comp = GetOwner()->FindComponentByClass(UTimeEventManagerComponent::StaticClass()))
	{
		timeEventManager = Cast<UTimeEventManagerComponent>(comp);
		timeEventManager->OnRewindFinished.AddUObject(this, &URecordableComponent::DestroyRewindCheckpoint);
	}

	if(GetOwner()->HasAuthority())
	{
		if (UActorComponent *comp = GetOwner()->FindComponentByClass(UDamageableCharacterComponent::StaticClass()))
		{
			damageableComponent = Cast<UDamageableCharacterComponent>(comp);
		}

		if (UActorComponent *comp = GetOwner()->FindComponentByClass(UAttackHitbox::StaticClass()))
		{
			attackHitBox = Cast<UAttackHitbox>(comp);
		}

		EventManager::OnRecordPressed.AddUObject(this, &URecordableComponent::StartRecording);
		EventManager::OnRecordReleased.AddUObject(this, &URecordableComponent::StopRecording);
		EventManager::OnCharacterDied.AddUObject(this, &URecordableComponent::CancelRecording);

		rewindManager = GetWorld()->GetGameState()->FindComponentByClass<URewindManagerComponent>();

		if (rewindManager && rewindManager->IsRecording())
		{
			StartRecording();
			relativeTime = rewindManager->GetRelativeTime();
			destroyAfterRewind = true;
		}
	}
}

void URecordableComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if(isRecording)
	{
		relativeTime += DeltaTime;

		rewindStack.Add(FSnapshotInfo(GetOwner()->GetActorRotation(),
									  GetOwner()->GetActorLocation(),
									  relativeTime,
									  attackHitBox? attackHitBox->CanDamage:true,
									  damageableComponent ? damageableComponent->Health : 0));
	}
}

void URecordableComponent::StartRecording(bool isFixedDuration, float duration)
{
	if(GetOwner()->HasAuthority())
	{
		RPC_StartRecording();
	}
}

void URecordableComponent::RPC_StartRecording_Implementation()
{
	relativeTime = 0;
	isRecording = true;
	rewindStack.Empty();

	if (GetOwner()->HasAuthority() && RewindCheckpointClass && !rewindCheckpoint)
	{
		rewindCheckpoint = GetWorld()->SpawnActor<AActor>(RewindCheckpointClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	}

	timeEventManager->OnRecordStarted.Broadcast();
	OnEnableRecordPostProcessRequested.Broadcast();
}

void URecordableComponent::StopRecording()
{
	if(GetOwner()->HasAuthority() && isRecording)
	{
		RPC_StopRecording();
	}
}

void URecordableComponent::RPC_StopRecording_Implementation()
{
	isRecording = false;

	if(timeEventManager)
	{
		timeEventManager->OnRecordStopped.Broadcast(rewindStack, destroyAfterRewind);
	}

	OnDisableRecordPostProcessRequested.Broadcast();
}

void URecordableComponent::DestroyRewindCheckpoint(const TArray<FSnapshotInfo> &stack)
{
	if (rewindCheckpoint)
	{
		rewindCheckpoint->Destroy();
		rewindCheckpoint = nullptr;
	}
}

void URecordableComponent::CancelRecording()
{
	if (GetOwner()->HasAuthority() && isRecording)
	{
		RPC_CancelRecording();
	}
}

void URecordableComponent::RPC_CancelRecording_Implementation()
{
	isRecording = false;

	if (rewindCheckpoint)
	{
		rewindCheckpoint->Destroy();
		rewindCheckpoint = nullptr;
	}

	OnDisableRecordPostProcessRequested.Broadcast();
}