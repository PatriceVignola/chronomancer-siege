// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HitBoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "DamagableHitbox.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKnockBackDelegate,FVector,ImpactNormal);
/**
 * 
 */
UCLASS(ClassGroup = (Collision), meta = (BlueprintSpawnableComponent))
class ACADEMIA2017_API UDamagableHitbox : public UHitBoxComponent
{
	GENERATED_BODY()
public:

	UDamagableHitbox();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FKnockBackDelegate OnKnockback;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageMultiplier = 1.0;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite)
	bool HasBeenHit  = false;

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite)
	FVector HitLocation = FVector(0,0,0);

	float timeSinceBeenHit = 1.f;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(Server,WithValidation,Reliable)
	void Server_OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(NetMulticast, Reliable)
	void RPC_ApplyDamage(UAttackHitbox *attackingHitBox, FVector pos);
};