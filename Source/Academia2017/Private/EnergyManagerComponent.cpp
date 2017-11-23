#include "Academia2017.h"
#include "EnergyManagerComponent.h"
#include "EventManager.h"
#include "Net/UnrealNetwork.h"

UEnergyManagerComponent::UEnergyManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnergyManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		GiveEnergy(InitialEnergy);
		EventManager::OnDeathRewindFinished.AddUObject(this, &UEnergyManagerComponent::RefillEnergy);
	}
}

void UEnergyManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEnergyManagerComponent, Energy);
}

void UEnergyManagerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (GetOwner()->HasAuthority())
	{
		GiveEnergy(EnergyRegenerationSpeed * DeltaTime);
	}
}

float UEnergyManagerComponent::GetRemainingEnergy()
{
	return Energy;
}

void UEnergyManagerComponent::ConsumeEnergy(float energyAmount)
{
	if (energyAmount <= 0) return;

	Energy -= energyAmount;

	if (Energy < 0)
	{
		Energy = 0;
	}

	OnEnergyChanged();
}

void UEnergyManagerComponent::GiveEnergy(float energyAmount)
{
	if (energyAmount <= 0) return;

	Energy += energyAmount;

	if (Energy > MaxEnergy)
	{
		Energy = MaxEnergy;
	}

	OnEnergyChanged();
}

void UEnergyManagerComponent::OnEnergyChanged()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_OnEnergyChanged();
	}
	else
	{
		Server_OnEnergyChanged();
	}
}

void UEnergyManagerComponent::Server_OnEnergyChanged_Implementation()
{
	RPC_OnEnergyChanged();
}

bool UEnergyManagerComponent::Server_OnEnergyChanged_Validate()
{
	return true;
}

void UEnergyManagerComponent::RPC_OnEnergyChanged_Implementation()
{
	EnergyChangedEvent(Energy);
}

float UEnergyManagerComponent::GetMaxEnergy()
{
	return MaxEnergy;
}

void UEnergyManagerComponent::RefillEnergy()
{
	GiveEnergy(MaxEnergy - Energy);
}