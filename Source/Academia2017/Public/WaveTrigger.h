#pragma once

#include "GameFramework/Actor.h"
#include "WarriorCharacter.h"
#include "MageCharacter.h"
#include "PillarGroup.h"
#include "Gate.h"
#include "Nest.h"
#include "WaveTrigger.generated.h"

USTRUCT()
struct ACADEMIA2017_API FSpawnInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	ANest *Nest;

	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	int32 WorkerCount;

	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	int32 TankCount;

	UPROPERTY(EditAnywhere, Category = SpawnInfo)
	int32 AssassinCount;
};

USTRUCT()
struct ACADEMIA2017_API FWaveInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = WaveInfo)
	TArray<FSpawnInfo> SpawnInfo;
};

UCLASS()
class ACADEMIA2017_API AWaveTrigger : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditInstanceOnly, Category = WaveTrigger)
	bool WarriorRequired = true;

	UPROPERTY(EditInstanceOnly, Category = WaveTrigger)
	bool MageRequired = true;

	UPROPERTY(EditInstanceOnly, Category = WaveTrigger)
	float TimeBetweenWaves = 5.f;

	UPROPERTY(EditInstanceOnly, Category = WaveTrigger)
	TArray<FWaveInfo> WaveInfo;

	UPROPERTY(EditInstanceOnly, Category = WaveTrigger)
	AGate *GateBefore;

	UPROPERTY(EditInstanceOnly, Category = WaveTrigger)
	AGate *GateAfter;

	UPROPERTY(EditInstanceOnly, Category = WaveTrigger)
	APillarGroup* PillarGroup;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent *TriggerZone;

	enum class EWaveTriggerState { NotStarted, DuringWave, BetweenWave, Finished, Frozen };

	struct FSavedInfo
	{
		bool warriorInside, mageInside, alreadyTriggered;
		int currentWaveIndex;
		float timeSinceWaveFinished;
		FDelegateHandle closeDoorHandle;
		EWaveTriggerState currentState;
	};

public:	
	AWaveTrigger();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Sounds)
	void PlaySpawnSound();

private:
	EWaveTriggerState currentState = EWaveTriggerState::NotStarted;

	FDelegateHandle closeDoorHandle;
	bool warriorInside = false;
	bool mageInside = false;
	bool alreadyTriggered = false;
	int currentWaveIndex = -1;
	float timeSinceWaveFinished = 0.f;
	FSavedInfo savedInfo;

	void StartFirstWave();
	void StartNextWave();
	void StopWave();
	void CheckAllEnemiesDead();

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ResetWaves();
	void FreezeWaves();
	void SaveWaveInfo(bool, float);
	void LoadWaveInfo();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlaySpawnSound();
};
