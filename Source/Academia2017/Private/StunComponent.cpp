// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "StunComponent.h"


// Sets default values for this component's properties
UStunComponent::UStunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


// Called when the game starts
void UStunComponent::BeginPlay()
{
	Super::BeginPlay();
	m_Timer = StunTime;
	// ...
	
}

void UStunComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UStunComponent, IsStunned);
}
// Called every frame
void UStunComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	if (GetOwner()->HasAuthority())
	{
		if (!IsStunned)
		{
			if (PillarGroupActor&& PillarGroupActor->IsActivated)
			{


				IsStunned = true;
				m_Timer = StunTime;
			}
		}
		else
		{
			m_Timer -= DeltaTime;
			if (m_Timer <= 0)
			{
				IsStunned = false;
				PillarGroupActor->ResetPillars();
			}
		}
	}
}

