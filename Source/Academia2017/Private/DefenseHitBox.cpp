// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "AttackHitbox.h"
#include "DefenseHitBox.h"


UDefenseHitBox::UDefenseHitBox()
{
	PrimaryComponentTick.bCanEverTick = false;
	HitBoxType = EHitboxType::DEFENSE;
	this->OnComponentBeginOverlap.AddDynamic(this, &UDefenseHitBox::OnBeginOverlap);
	//this->OnComponentEndOverlap.AddDynamic(this, &UDefenseHitBox::OnEndOverlap);
}

void UDefenseHitBox::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetOwner()->HasAuthority())
	{
		if (GetOwner()->GetClass() != OtherComp->GetOwner()->GetClass())
		{
			if (UAttackHitbox* attackingHitBox = Cast<UAttackHitbox>(OtherComp))
			{
				if (attackingHitBox->HitBoxType == EHitboxType::ATTACK && attackingHitBox->CanDamage)
				{
					//CouldDamage = attackingHitBox->CanDamage;
					UE_LOG(LogTemp, Warning, TEXT("Blocked damage  !!!"));
					attackingHitBox->HasHitShield = true;

				}
			}
		}
	}
	else
	{
		Server_OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}

}

void UDefenseHitBox::Server_OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

bool UDefenseHitBox::Server_OnBeginOverlap_Validate(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	return true;
}