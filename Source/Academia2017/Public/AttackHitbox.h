// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HitBoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "AttackHitbox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayImpactSoundRequested);

/**
 * 
 */
UCLASS(ClassGroup = (Collision), meta = (BlueprintSpawnableComponent))
class ACADEMIA2017_API UAttackHitbox : public UHitBoxComponent
{
	GENERATED_BODY()
	// Sets default values for this component's properties
public:
	UAttackHitbox();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float BaseDamage = 10.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float  KnockBackMultiplier = 5.0f;
	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	bool CanDamage = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	bool HasHitShield = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	bool HasKnockBackEffect = false;
	UFUNCTION(BlueprintCallable,Category = Damage)
	void SetCanDamage(bool damage);
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_SetCanDamage(bool damage);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void RPC_SetCanDamage(bool damage);

	UPROPERTY(BlueprintAssignable)
	FPlayImpactSoundRequested OnPlayImpactSoundRequested;
	
};
