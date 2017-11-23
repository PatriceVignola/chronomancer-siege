#pragma once

#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "EnergyCrystal.h"
#include "TimeEventManagerComponent.h"
#include "DamageableCharacterComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthChangedDelegate, float, health);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMageHealthChangedDelegate, float, currentHealth,float,MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWarriorHealthChangedDelegate, float, currentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDiedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRevivedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDamageTakenDelegate);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACADEMIA2017_API UDamageableCharacterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageableCharacterComponent();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(EditDefaultsOnly)
	bool CanGameOver = true;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,ReplicatedUsing = NotifyHealthChanged)
	float Health = 100;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Replicated)
	bool IsDead;

	UPROPERTY(BlueprintAssignable)
	FMageHealthChangedDelegate OnMageHealthChange;

	UPROPERTY(BlueprintAssignable)
	FWarriorHealthChangedDelegate OnWarriorHealthChange;

	UPROPERTY(BlueprintAssignable)
	FHealthChangedDelegate OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FDamageTakenDelegate OnDamageTaken;

	UPROPERTY(BlueprintAssignable)
	FDiedDelegate OnDied;

	UPROPERTY(BlueprintAssignable)
	FRevivedDelegate OnRevived;

	UPROPERTY(EditAnywhere, Category = Energy)
	bool DropsEnergy;

	/*Chance of dropping an energy crystal, in a scale from 0 to 1*/
	UPROPERTY(EditAnywhere, Category = Energy)
	float EnergyDropChance = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = Energy)
	TSubclassOf<AEnergyCrystal> EnergyCrystalClass = nullptr;

	UFUNCTION(BlueprintCallable, Category = Damage)
	void Damage(float damagePoints);

	UFUNCTION(BlueprintCallable, Category = Damage)
	void SetHealth(float newHealth);

private:
	UInputComponent *inputComponent = nullptr;
	UTimeEventManagerComponent *timeEventManager = nullptr;
	bool invulnerable = false;

	void Kill();
	void SetDeadState();
	void SetAliveState();
	void SetVulnerableState();
	void SetInvulnerableState();
	void SetInvulnerableHack();
	void RefillLife();

	UFUNCTION()
	void NotifyWarriorHealthChanged(float currentHealth,float OtherPlayerMaxHealth);

	UFUNCTION()
	void NotifyMageHealthChanged(float currentHealth, float OtherPlayerMaxHealth);

	UFUNCTION()
	void NotifyHealthChanged();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_NotifyDied();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_NotifyRevived();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestKill();

	UFUNCTION(Client, Reliable)
	void RPC_NotifyDamageTaken();

	UFUNCTION(Server,WithValidation, Reliable)
	void Server_NotifyWarriorHealthChanged(float currentHealth, float OtherPlayerMaxHealth);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_NotifyWarriorHealthChanged(float currentHealth, float OtherPlayerMaxHealth);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_NotifyMageHealthChanged(float currentHealth, float OtherPlayerMaxHealth);

	UFUNCTION(Server,WithValidation, Reliable)
	void Server_NotifyMageHealthChanged(float currentHealth, float OtherPlayerMaxHealth);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetInvulnerableHack();
};
