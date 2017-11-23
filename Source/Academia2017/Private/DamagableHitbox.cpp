// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "Net/UnrealNetwork.h"
#include "AttackHitbox.h"
#include "DefenseHitBox.h"
#include "Components/PrimitiveComponent.h"
#include "DamageableCharacterComponent.h"
#include "DrawDebugHelpers.h"
#include "DamagableHitbox.h"

UDamagableHitbox::UDamagableHitbox()
{
	PrimaryComponentTick.bCanEverTick = true;
	HitBoxType = EHitboxType::DAMAGABLE;
	this->OnComponentBeginOverlap.AddDynamic(this, &UDamagableHitbox::OnBeginOverlap);
}

void UDamagableHitbox::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDamagableHitbox, HitLocation);
	DOREPLIFETIME(UDamagableHitbox, HasBeenHit);
}

void UDamagableHitbox::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasBeenHit)
	{
		timeSinceBeenHit -= DeltaTime;

		if (timeSinceBeenHit)
		{
			timeSinceBeenHit = 1.f;
			HasBeenHit = false;
		}
	}
}

void UDamagableHitbox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetOwner()->HasAuthority())
	{
		if (OtherComp && GetOwner()->GetClass() != OtherComp->GetOwner()->GetClass())
		{
			if (UAttackHitbox* attackingHitBox = Cast<UAttackHitbox>(OtherComp))
			{
				if (!attackingHitBox->HasHitShield)
				{
					if (attackingHitBox->HitBoxType == EHitboxType::ATTACK && attackingHitBox->CanDamage)
					{
						if (UActorComponent* dmg = GetOwner()->FindComponentByClass(UDamageableCharacterComponent::StaticClass()))
						{
							RPC_ApplyDamage(attackingHitBox, OtherComp->GetComponentLocation());
							if (attackingHitBox->HasKnockBackEffect)
							{
								FVector KnockBackDirection =  (GetOwner()->GetActorLocation()-OtherActor->GetActorLocation()).GetSafeNormal();
								KnockBackDirection.Z = 10.0f;
								OnKnockback.Broadcast(KnockBackDirection*attackingHitBox->KnockBackMultiplier);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		Server_OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}

void UDamagableHitbox::Server_OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

bool UDamagableHitbox::Server_OnBeginOverlap_Validate(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	return true;
}

void UDamagableHitbox::RPC_ApplyDamage_Implementation(UAttackHitbox *attackingHitBox, FVector pos)
{
	if (attackingHitBox)
	{
		if (UDamageableCharacterComponent* damageable = GetOwner()->FindComponentByClass<UDamageableCharacterComponent>())
		{
			damageable->Damage(attackingHitBox->BaseDamage * DamageMultiplier);
			this->HasBeenHit = true;
			this->HitLocation = pos;

			attackingHitBox->OnPlayImpactSoundRequested.Broadcast();
		}
	}
}