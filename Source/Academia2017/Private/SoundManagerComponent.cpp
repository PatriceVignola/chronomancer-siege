#include "Academia2017.h"
#include "EventManager.h"
#include "SoundManagerComponent.h"

USoundManagerComponent::USoundManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USoundManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	EventManager::OnRecordPressed.AddUObject(this, &USoundManagerComponent::OnRecordPressed);
	EventManager::OnRecordReleased.AddUObject(this, &USoundManagerComponent::OnRecordReleased);
	EventManager::OnRewindFinished.AddUObject(this, &USoundManagerComponent::OnRewindFinished);
	EventManager::OnFightStarted.AddUObject(this, &USoundManagerComponent::OnFightStarted);
	EventManager::OnFightEnded.AddUObject(this, &USoundManagerComponent::OnFightEnded);
	EventManager::OnDeathMusicLayerRequested.AddUObject(this, &USoundManagerComponent::OnDeathMusicLayerRequested);
	EventManager::OnDeathRewindFinished.AddUObject(this, &USoundManagerComponent::OnDeathRewindFinished);
	EventManager::OnRecordCancelled.AddUObject(this, &USoundManagerComponent::OnRewindFinished);
}

void USoundManagerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void USoundManagerComponent::OnRecordPressed(bool isFixedDuration, float duration)
{
	if (GetOwner()->HasAuthority())
	{
		RPC_PlayRecordLayer();
	}
}

void USoundManagerComponent::OnRecordReleased()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_PlayRewindLayer();
	}
}

void USoundManagerComponent::OnRewindFinished()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_PlayMusicLayer();
	}
}

void USoundManagerComponent::OnFightStarted()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_PlayFightLayer();
	}
}

void USoundManagerComponent::OnFightEnded()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_PlayExplorationLayer();
	}
}

void USoundManagerComponent::OnDeathMusicLayerRequested()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_PlayDeathRewindLayer();
	}
}

void USoundManagerComponent::OnDeathRewindFinished()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_StopDeathRewindLayer();
	}
}

void USoundManagerComponent::RPC_PlayRecordLayer_Implementation()
{
	OnRecordLayerRequested();
}

void USoundManagerComponent::RPC_PlayRewindLayer_Implementation()
{
	OnRewindLayerRequested();
}

void USoundManagerComponent::RPC_PlayMusicLayer_Implementation()
{
	if (fighting)
	{
		OnFightLayerRequested();
	}
	else
	{
		PlayExplorationMusic();
	}
}

void USoundManagerComponent::RPC_PlayFightLayer_Implementation()
{
	fighting = true;
	OnFightLayerRequested();
}

void USoundManagerComponent::RPC_PlayExplorationLayer_Implementation()
{
	fighting = false;
	PlayExplorationMusic();
}

void USoundManagerComponent::RPC_PlayDeathRewindLayer_Implementation()
{
	OnPlayDeathRewindLayerRequested();
}

void USoundManagerComponent::RPC_StopDeathRewindLayer_Implementation()
{
	OnStopDeathRewindLayerRequested();
}