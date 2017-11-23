#pragma once

#include "GameFramework/Actor.h"
#include "RewindManagerComponent.h"
#include "EnergyCrystal.generated.h"

UCLASS()
class ACADEMIA2017_API AEnergyCrystal : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnergyCrystal, meta = (AllowPrivateAccess = "true"))
	UBoxComponent *Collider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnergyCrystal, meta = (AllowPrivateAccess = "true"))
	float EnergyValue = 5.f;

	/*Lifetime of the crystal (0 is infinite)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnergyCrystal, meta = (AllowPrivateAccess = "true"))
	float Duration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnergyCrystal, meta = (AllowPrivateAccess = "true"))
	bool Rewindable = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnergyCrystal, meta = (AllowPrivateAccess = "true"))
	bool DropsOnlyOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnergyCrystal, meta = (AllowPrivateAccess = "true"))
	bool RespawnsInfinitely = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = EnergyCrystal, meta = (AllowPrivateAccess = "true"))
	float RespawnCooldown = 5.f;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem *DeathParticlesTemplate;

	UPROPERTY(EditAnywhere, Category = EnergyCrystal)
	float RotationSpeed = 45.f;

public:	
	AEnergyCrystal();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent *ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Sounds)
	void PlayDestroySound();

private:
	URewindManagerComponent *rewindManager;
	float recordElapsedTime = 0.f;
	float timeSinceSpawned = 0.f;
	bool recording = false;
	bool rewinding = false;
	bool dead = false;
	bool alreadyDroppedOnce = false;
	bool dontDestroyAfterDeathRewind = false;
	float deadTimeStamp = 0.f;
	float timeSinceRespawned = 0.f;
	FRotator angularVelocity;

	void NoticeRecordStarted(bool isFixedDuration, float duration);
	void NoticeRecordStopped();
	void DestroyEnergyCrystal();
	void SetDeadState();
	void SetAliveState();
	void OnGameOver();
	bool IsOverlappingEnergyManager();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_NoticeRecordStarted();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_NoticeRecordStopped();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_SetAngularVelocity(const FRotator &newAngularVelocity);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_SpawnParticlesAndSound();
	
	void SetDontDestroyAfterDeathRewind();
	void CheckIfShouldDestroy();

	UFUNCTION(NetMulticast, Unreliable)
	void RPC_Rotate(float deltaTime);

	UFUNCTION(NetMulticast, Unreliable)
	void RPC_RotateReverse(float rewindDelta);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_SetDefaultAngle(const FRotator& rotator);
};
