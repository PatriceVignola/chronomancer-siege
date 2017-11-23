// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Controller.h"
#include "Nest.generated.h"

UCLASS()
class ACADEMIA2017_API ANest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANest();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	bool RemainEnemies();

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SpawnEnemies(int workerCount = 0, int tankCount = 0, int assassinCount = 0);

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SpawnBoss();
	UFUNCTION(BlueprintCallable,Server,Reliable,WithValidation, Category = "Enemy")
	void Server_SpawnBoss();
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnemyBase> BossClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnemyBase> WorkerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnemyBase> TankClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnemyBase> AssassinClass;

private:
	UPROPERTY()
	TArray<AEnemyBase *> spawnedEnemies;

	void OnDeathRewindFinished();
};
