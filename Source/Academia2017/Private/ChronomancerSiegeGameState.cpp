#include "Academia2017.h"
#include "EventManager.h"
#include "ChronomancerSiegeGameState.h"

AChronomancerSiegeGameState::AChronomancerSiegeGameState()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
}

void AChronomancerSiegeGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		EventManager::OnRecordPressed.AddUObject(this, &AChronomancerSiegeGameState::OnRecordPressed);
		EventManager::OnRecordReleased.AddUObject(this, &AChronomancerSiegeGameState::OnRecordReleased);
		EventManager::OnRewindFinished.AddUObject(this, &AChronomancerSiegeGameState::OnRewindFinished);
	}
}

void AChronomancerSiegeGameState::OnRecordPressed(bool, float)
{
	if (HasAuthority())
	{
		RPC_OnRecordPressed();
	}
}

void AChronomancerSiegeGameState::OnRecordReleased()
{
	if (HasAuthority())
	{
		RPC_OnRecordReleased();
	}
}

void AChronomancerSiegeGameState::OnRewindFinished()
{
	if (HasAuthority())
	{
		RPC_OnRewindFinished();
	}
}

void AChronomancerSiegeGameState::RPC_OnRecordPressed_Implementation()
{
	EnableRecordPostProcess();
}

void AChronomancerSiegeGameState::RPC_OnRecordReleased_Implementation()
{
	DisableRecordPostProcess();
	EnableRewindPostProcess();
}

void AChronomancerSiegeGameState::RPC_OnRewindFinished_Implementation()
{
	DisableRewindPostProcess();
}