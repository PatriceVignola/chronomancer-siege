// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "Net/UnrealNetwork.h"
#include "AttackHitbox.h"

UAttackHitbox::UAttackHitbox()
{
	HitBoxType = EHitboxType::ATTACK;
}

void UAttackHitbox::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAttackHitbox, CanDamage);
}
void UAttackHitbox::SetCanDamage(bool damage)
{
	/*
	if (!GetOwner()->HasAuthority())
	{
		Server_SetCanDamage(damage);
	}*/

	if (GetOwner()->HasAuthority())
	{
		RPC_SetCanDamage(damage);
	}
}
void UAttackHitbox::Server_SetCanDamage_Implementation(bool damage)
{
	RPC_SetCanDamage(damage);
}
bool UAttackHitbox::Server_SetCanDamage_Validate(bool damage)
{
	return true;
}
void UAttackHitbox::RPC_SetCanDamage_Implementation(bool damage)
{
	this->CanDamage = damage;
}
bool UAttackHitbox::RPC_SetCanDamage_Validate(bool damage)
{
	return true;
}
