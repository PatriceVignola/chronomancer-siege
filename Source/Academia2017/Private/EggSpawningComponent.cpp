// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "EggSpawningComponent.h"


// Sets default values for this component's properties
UEggSpawningComponent::UEggSpawningComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEggSpawningComponent::BeginPlay()
{
	Super::BeginPlay();
	m_Timer = TimeBetweenEggSpawns;
	// ...
	
}
void UEggSpawningComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEggSpawningComponent, IsActivated);
}

// Called every frame
void UEggSpawningComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (IsActivated)
	{
		m_Timer -= DeltaTime;
		//UE_LOG(LogTemp, Warning, TEXT("Time Remaining : %f"), m_Timer);
		if (m_Timer <= 0.0f)
		{
			if(GetOwner()->HasAuthority())
				Server_SpawnEgg();
			m_Timer = TimeBetweenEggSpawns;
		}

	}
	// ...
}
void UEggSpawningComponent::Server_SpawnEgg_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnEgg"));
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	GetWorld()->SpawnActor<ABossEgg>(EggSpawnClass, GetComponentLocation(), FRotator::ZeroRotator, param);
}
bool UEggSpawningComponent::Server_SpawnEgg_Validate()
{
	return true;
}

