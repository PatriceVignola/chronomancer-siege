// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "AttackHitbox.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "ActivableHitbox.h"

UActivableHitbox::UActivableHitbox()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActivableHitbox::BeginPlay()
{
	HitBoxType = EHitboxType::ACTIVATABLE;
	OnComponentBeginOverlap.AddDynamic(this, &UActivableHitbox::OnBeginOverlap);
}

void UActivableHitbox::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UActivableHitbox, IsActivated);
}

void UActivableHitbox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetOwner()->HasAuthority())
	{
		if (UAttackHitbox *attackingHitBox = Cast<UAttackHitbox>(OtherComp))
		{
			if (!attackingHitBox->HasHitShield && attackingHitBox->HitBoxType == EHitboxType::ATTACK && attackingHitBox->CanDamage)
			{
				RPC_Activate(OtherComp->GetComponentLocation());
			}
		}
	}
}

void UActivableHitbox::RPC_Activate_Implementation(FVector hitLoc)
{
	IsActivated = true;
	HitLocation = hitLoc;
}