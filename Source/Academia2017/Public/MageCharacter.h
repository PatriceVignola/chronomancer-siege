// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "PlayableCharacter.h"
#include "HitBoxComponent.h"
#include "HealingDome.h"
#include "MageCharacter.generated.h"

UCLASS(config = Game)
class AMageCharacter : public APlayableCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = SlowDome)
	TSubclassOf<AHealingDome> HealingDomeClass = nullptr;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	class UAnimInstance* AnimInstance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mage, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* MagicCastingMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mage, meta = (AllowPrivateAccess = "true"))
	float RunMultiplier = 2;

public:
	AMageCharacter();
	void Tick(float DeltaTime) override;

	void BeginPlay() override;

	const bool isCameraRotationTriggered() const;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	void RequestCastHealingDome();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestCastHealingDome();

	void CastHealingDome();

	void ActivateHealingParticles();

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	void TriggerCameraRotation();
	void RemoveCameraRotation();

	UFUNCTION(BlueprintImplementableEvent, Category = HealingDome)
	void PlayHealingSound();

	UFUNCTION(BlueprintImplementableEvent, Category = HealingDome)
	void BPActivateHealingParticles();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void RPC_StartRunningOnServer();

	UFUNCTION(Server, Reliable, WithValidation)
	void RPC_StopRunningOnServer();

	void NotifyMageSpawned();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_NotifyMageSpawned();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayHealingSound();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_ActivateHealingParticles();
};

