#include "Academia2017.h"
#include "CheatTeleporterComponent.h"

UCheatTeleporterComponent::UCheatTeleporterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCheatTeleporterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCheatTeleporterComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// Executed only once
	if (!inputComponent && GetOwner()->InputComponent)
	{
		inputComponent = GetOwner()->InputComponent;

		inputComponent->BindAction("Teleport_DEBUG_1", IE_Pressed, this, &UCheatTeleporterComponent::TeleportToLocation1);
		inputComponent->BindAction("Teleport_DEBUG_2", IE_Pressed, this, &UCheatTeleporterComponent::TeleportToLocation2);
		inputComponent->BindAction("Teleport_DEBUG_3", IE_Pressed, this, &UCheatTeleporterComponent::TeleportToLocation3);
		inputComponent->BindAction("Teleport_DEBUG_4", IE_Pressed, this, &UCheatTeleporterComponent::TeleportToLocation4);
		inputComponent->BindAction("Teleport_DEBUG_5", IE_Pressed, this, &UCheatTeleporterComponent::TeleportToLocation5);
		inputComponent->BindAction("Teleport_DEBUG_6", IE_Pressed, this, &UCheatTeleporterComponent::TeleportToLocation6);
		inputComponent->BindAction("Teleport_DEBUG_7", IE_Pressed, this, &UCheatTeleporterComponent::TeleportToLocation7);
		inputComponent->BindAction("Teleport_DEBUG_8", IE_Pressed, this, &UCheatTeleporterComponent::TeleportToLocation8);
	}
}

void UCheatTeleporterComponent::TeleportToLocation1()
{
	TeleportToLocation(0);
}

void UCheatTeleporterComponent::TeleportToLocation2()
{
	TeleportToLocation(1);
}

void UCheatTeleporterComponent::TeleportToLocation3()
{
	TeleportToLocation(2);
}

void UCheatTeleporterComponent::TeleportToLocation4()
{
	TeleportToLocation(3);
}

void UCheatTeleporterComponent::TeleportToLocation5()
{
	TeleportToLocation(4);
}

void UCheatTeleporterComponent::TeleportToLocation6()
{
	TeleportToLocation(5);
}

void UCheatTeleporterComponent::TeleportToLocation7()
{
	TeleportToLocation(6);
}

void UCheatTeleporterComponent::TeleportToLocation8()
{
	TeleportToLocation(7);
}

void UCheatTeleporterComponent::TeleportToLocation(int index)
{
	if (GetOwner()->HasAuthority())
	{
		if (TeleportLocations.Num() > index)
		{
			GetOwner()->SetActorLocation(TeleportLocations[index]);
		}
	}
	else
	{
		Server_RequestTeleportToLocation(index);
	}
}

void UCheatTeleporterComponent::Server_RequestTeleportToLocation_Implementation(int index)
{
	if (GetOwner()->HasAuthority())
	{
		TeleportToLocation(index);
	}
}

bool UCheatTeleporterComponent::Server_RequestTeleportToLocation_Validate(int index)
{
	return true;
}