#pragma once

#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "HitBoxComponent.h"
#include "AttackHitbox.h"
#include "PlayableCharacter.h"
#include "TimeEventManagerComponent.h"
#include "WarriorCharacter.generated.h"

UCLASS(config=Game)
class AWarriorCharacter : public APlayableCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Warrior, meta = (AllowPrivateAccess = "true"))
	float RunMultiplier = 2;

	UPROPERTY(BlueprintReadOnly, Category = Warrior, meta = (AllowPrivateAccess = "true"))
	FVector ForwardDirection;

	UPROPERTY(BlueprintReadOnly, Category = Warrior, meta = (AllowPrivateAccess = "true"))
	FVector RightDirection;

public:
	AWarriorCharacter();
	void Tick(float DeltaTime) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Time, meta = (AllowPrivateAccess = "true"))
	bool isClone = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HeavyAttackDamage = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float LightAttackDamage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool CanCombo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float AttackCooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> LightAttackAnims;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> HeavyAttackAnims;

	UFUNCTION()
	void DoLightAttack();

	void DoLightAttack(FVector direction);

	UFUNCTION()
	void DoHeavyAttack();

	UFUNCTION()
	void ActivateHealingParticles();

	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector AnimVelocity;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	UFUNCTION(BlueprintImplementableEvent, Category = Attack)
	void PlayLightAttackSound(int attackIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = Attack)
	void PlayHeavyAttackSound(int attackIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = HealingDome)
	void BPActivateHealingParticles();

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	UAnimInstance* AnimInstance = nullptr;
	UAttackHitbox *attackHitbox = nullptr;
	UTimeEventManagerComponent *timeEventManager = nullptr;
	FVector LightAttackQueuedDirection;
	FVector inputDirection;
	int LightAttackIndex = 0;
	int HeavyAttackIndex = 0;
	float m_Timer = 0.0f;
	bool LightAttackQueued = false;
	bool HeavyAttackQueued = false;
	bool m_isInFirstAttack = false;
	bool m_IsOnCooldown = false;
	bool frameFirstInput = true;
	bool blockMovementInput = false;

	UFUNCTION()
	void AttackMontageBleedingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestDoHeavyAttack();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestDoLightAttack(FVector lastInputDir);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayHeavyAttackAnim(int AttackIndex);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayLightAttackAnim(int AttackIndex, FVector direction);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_ActivateHealingParticles();
};

