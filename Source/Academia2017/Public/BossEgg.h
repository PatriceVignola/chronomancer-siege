#pragma once

#include "GameFramework/Actor.h"
#include "ActivableHitbox.h"
#include "EnemyBase.h"
#include "BossEgg.generated.h"

class UDestructibleComponent;

UCLASS()
class ACADEMIA2017_API ABossEgg : public AActor
{
	GENERATED_BODY()
	
public:	
	ABossEgg();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	void BreakEgg();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float DelayBeforeSpawn = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EnemyCount = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UActivableHitbox* BodyHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UDestructibleComponent* DestructableMesh;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnemyBase> EnemySpawnedClass;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Sounds)
	void PlayCrackSound();

private:
	bool m_IsNeutralized = false;
	bool shouldDestroy = false;
	bool broken = false;
	float destroyTimer = 0.f;
	float m_Timer = 0.0f;
	
	void SpawnEnemies();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayCrackSound();
};
