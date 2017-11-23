#pragma once

#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "EnemyBase.generated.h"

UENUM(BlueprintType)
enum class EAIStates : uint8
{
	COMBAT 		UMETA(DisplayName = "COMBAT"),
	WANDER 	UMETA(DisplayName = "WANDER"),
	PATROL 	UMETA(DisplayName = "PATROL"),
	STAGGER 	UMETA(DisplayName = "STAGGER"),
};
UCLASS()
class ACADEMIA2017_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RunSpeed;

	UPROPERTY( EditDefaultsOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* UnderBellyAttack;
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* MeleeRangeBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TArray <AActor*> PatrolPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool isAttacking = false;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "AI", meta = (AllowPrivateAcess = true))
	AActor* GetTarget(TArray<AActor*> VisibleCharacters);
	UFUNCTION(BlueprintCallable, Category = "AI", meta = (AllowPrivateAcess = true))
	FVector CalculateTacticalRetreat(TArray<AActor*> EnemyCharacters, TArray<AActor*> FriendlyCharacters,float DistanceBehindTroop,float FloorZ ,float MaxDistance);
	

	UFUNCTION(BlueprintCallable, Category = "Combat", meta = (AllowPrivateAcess = true))
	void Attack(bool isUnderBelly);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Attack(bool isUnderBelly = false);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void RPC_Attack(bool isUnderBelly = false);

	UFUNCTION()
	void AttackMontageBleedingOut(UAnimMontage* Montage, bool bInterrupted);
private:
	class UAnimInstance* AnimInstance;
};
