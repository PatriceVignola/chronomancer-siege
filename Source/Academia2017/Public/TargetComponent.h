#pragma once

#include "Components/ActorComponent.h"
#include "SlowDome.h"
#include "EnergyManagerComponent.h"
#include "TargetComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class ACADEMIA2017_API UTargetComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = SlowDome)
	TSubclassOf<ASlowDome> SlowDomeClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = SlowDome)
	UCurveFloat *SpeedCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = SlowDome)
	float MaxRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = SlowDome)
	float InitialScale = 0.f;
	
	UPROPERTY(EditDefaultsOnly, Category = SlowDome)
	float FinalScale = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = SlowDome)
	float MageMovementRatioDuringCast = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = SlowDome)
	UAnimMontage* MagicCastingMontage;

public:	
	UTargetComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = TargetComponent)
	void PlayCastSound();

private:
	const float defaultSpeed = 500.f;

	USkeletalMeshComponent *mesh = nullptr;
	UCharacterMovementComponent *charMovement = nullptr;
	UEnergyManagerComponent *energyManager = nullptr;
	UInputComponent *inputComponent = nullptr;
	UAnimInstance* animInstance = nullptr;
	ASlowDome *currentSlowDome = nullptr;
	float remainingSlowDomeCooldown = 0.f;
	float timeSinceCast = 0.f;
	float normalSpeed = 0.f;

	void RequestStartCastingSlowDome();
	void RequestStopCastingSlowDome();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestStartCastingSlowDome();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestStopCastingSlowDome();

	void StartCastingSlowDome();
	void StopCastingSlowDome();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayCastAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StopCastAnimation();
};
