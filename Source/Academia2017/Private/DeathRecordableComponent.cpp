#include "Academia2017.h"
#include "DeathRecordableComponent.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "EventManager.h"

UDeathRecordableComponent::UDeathRecordableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDeathRecordableComponent::BeginPlay()
{
	Super::BeginPlay();

	if(UActorComponent *comp = GetOwner()->FindComponentByClass(UTimeEventManagerComponent::StaticClass()))
	{
		timeEventManager = Cast<UTimeEventManagerComponent>(comp);
	}

	rewindManager = GetWorld()->GetGameState()->FindComponentByClass<UDeathRewindManagerComponent>();

	if(GetOwner()->HasAuthority())
	{
		if (UActorComponent *comp = GetOwner()->FindComponentByClass(UDamageableCharacterComponent::StaticClass()))
		{
			damageableComponent = Cast<UDamageableCharacterComponent>(comp);
		}

		EventManager::OnCheckpointReached.AddUObject(this, &UDeathRecordableComponent::StartRecording);
		EventManager::OnDeathRewindFinished.AddUObject(this, &UDeathRecordableComponent::StartRecording);
		EventManager::OnGameOver.AddUObject(this, &UDeathRecordableComponent::StopRecording);
	}
}

void UDeathRecordableComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if(IsRecording())
	{
		rewindStack.Add(FDeathSnapshotInfo(GetOwner()->GetActorRotation(),
									  GetOwner()->GetActorLocation(),
									  rewindManager->GetRelativeTime(),
									  damageableComponent ? damageableComponent->Health : 0));
	}
}

void UDeathRecordableComponent::StartRecording()
{
	if(GetOwner()->HasAuthority())
	{
		RPC_StartRecording();
	}
}

void UDeathRecordableComponent::RPC_StartRecording_Implementation()
{
	rewindStack.Empty();
}

void UDeathRecordableComponent::StopRecording()
{
	if(GetOwner()->HasAuthority())
	{
		RPC_StopRecording();
	}
}

void UDeathRecordableComponent::RPC_StopRecording_Implementation()
{
	if(timeEventManager)
	{
		timeEventManager->OnDeathRecordStopped.Broadcast(rewindStack);
	}
}

bool UDeathRecordableComponent::IsRecording()
{
	return rewindManager ? rewindManager->IsRecording() : false;
}