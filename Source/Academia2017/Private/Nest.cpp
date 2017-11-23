#include "Academia2017.h"
#include "Nest.h"
#include "Engine.h"
#include "EventManager.h"
#include "DamageableCharacterComponent.h"

ANest::ANest()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANest::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		EventManager::OnDeathRewindFinished.AddUObject(this, &ANest::OnDeathRewindFinished);
	}
}

void ANest::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

bool ANest::RemainEnemies()
{
	for (AEnemyBase *enemy : spawnedEnemies)
	{
		if (!enemy) continue;

		if (UDamageableCharacterComponent *damageable = enemy->FindComponentByClass<UDamageableCharacterComponent>())
		{
			if (!damageable->IsDead)
			{
				return true;
			}
		}
	}

	return false;
}

void ANest::SpawnBoss()
{
	if (HasAuthority())
	{
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AEnemyBase>(BossClass, GetActorLocation(), GetActorRotation(), params);
	}
	else
	{
		Server_SpawnBoss();
	}
}

void ANest::Server_SpawnBoss_Implementation()
{
	SpawnBoss();
}

bool ANest::Server_SpawnBoss_Validate()
{
	return true;
}

void ANest::SpawnEnemies(int workerCount, int tankCount, int assassinCount)
{
	if(HasAuthority())
	{
		FVector spawnerPos = GetActorLocation();

		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if(WorkerClass)
		{
			for(int i = 0; i < workerCount; ++i)
			{
				AEnemyBase *newEnemy = GetWorld()->SpawnActor<AEnemyBase>(WorkerClass, spawnerPos, FRotator::ZeroRotator, param);
				spawnedEnemies.Add(newEnemy);
			}
		}

		if(TankClass)
		{
			for(int i = 0; i < tankCount; ++i)
			{
				AEnemyBase *newEnemy = GetWorld()->SpawnActor<AEnemyBase>(TankClass, spawnerPos, FRotator::ZeroRotator, param);
				spawnedEnemies.Add(newEnemy);
			}
		}

		if(AssassinClass)
		{
			for(int i = 0; i < assassinCount; ++i)
			{
				AEnemyBase *newEnemy = GetWorld()->SpawnActor<AEnemyBase>(AssassinClass, spawnerPos, FRotator::ZeroRotator, param);
				spawnedEnemies.Add(newEnemy);
			}
		}
	}
}

void ANest::OnDeathRewindFinished()
{
	for (int i = 0; i < spawnedEnemies.Num(); i++)
	{
		spawnedEnemies.RemoveAt(i);
	}
}