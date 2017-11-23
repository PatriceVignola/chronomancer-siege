// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "EnergyManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnergyChangedOnClient, float, NewEnergy);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UEnergyManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Energy, meta = (AllowPrivateAccess = "true"))
	float MaxEnergy = 100;

	UPROPERTY(ReplicatedUsing = OnEnergyChanged)
	float Energy = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float EnergyRegenerationSpeed = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float InitialEnergy = 0.f;

public:	
	// Sets default values for this component's properties
	UEnergyManagerComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	float GetRemainingEnergy();
	void ConsumeEnergy(float energyAmount);
	void GiveEnergy(float energyAmount);
	float GetMaxEnergy();

	UPROPERTY(BlueprintAssignable)
	FEnergyChangedOnClient OnEnergyChangedEvent;

	UFUNCTION(BlueprintImplementableEvent, Category = EnergyManager)
	void EnergyChangedEvent(float EnergyAmount);

private:
	UFUNCTION()
	void RefillEnergy();

	UFUNCTION()
	void OnEnergyChanged();

	UFUNCTION(Server,WithValidation,Reliable)
	void Server_OnEnergyChanged();

	UFUNCTION(NetMulticast,Reliable)
	void RPC_OnEnergyChanged();
	
};
