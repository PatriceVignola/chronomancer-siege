// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HealingDome.generated.h"

class AWarriorCharacter;
class AMageCharacter;

UCLASS()
class ACADEMIA2017_API AHealingDome : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent *Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent *HealingZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent *HealingParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HealingDome, meta = (AllowPrivateAccess = "true"))
	float HealingAmount = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HealingDome, meta = (AllowPrivateAccess = "true"))
	float SpellDuration = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HealingDome, meta = (AllowPrivateAccess = "true"))
	float EnergyCost = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HealingDome, meta = (AllowPrivateAccess = "true"))
	float DamageAmount = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HealingDome, meta = (AllowPrivateAccess = "true"))
	float DelayBetweenStagger = 1.5f;

public:	
	// Sets default values for this actor's properties
	AHealingDome();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	float GetDomeCooldown();
	float GetEnergyCost();
	void StartRemoval();
	void SetShouldntHeal();

	struct ActorToDamage {
		AActor* actor = nullptr;
		float timeCount = 0.f;
		bool isStaggered = false;
	};

private:
	UFUNCTION()
	void HealPlayer(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void StartDamageEnemy(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void StopDamageEnemy(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TArray<ActorToDamage> m_actorsToDamage;
	AWarriorCharacter* m_warriorCharacter = nullptr;
	AMageCharacter* m_mageCharacter = nullptr;
	bool shouldntHeal = false;
	bool m_passFirstTick = false;
};

inline bool operator==(const AHealingDome::ActorToDamage& lhs, const AHealingDome::ActorToDamage& rhs) {
	return lhs.actor == rhs.actor;
}

inline bool operator!=(const AHealingDome::ActorToDamage& lhs, const AHealingDome::ActorToDamage& rhs) {
	return !operator==(lhs, rhs);
}