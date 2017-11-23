#include "Academia2017.h"
#include "EventManager.h"
#include "WaveTrigger.h"

AWaveTrigger::AWaveTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	RootComponent = TriggerZone;
}

void AWaveTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &AWaveTrigger::BeginOverlap);
		TriggerZone->OnComponentEndOverlap.AddDynamic(this, &AWaveTrigger::EndOverlap);

		EventManager::OnDeathRewindFinished.AddUObject(this, &AWaveTrigger::ResetWaves);
		EventManager::OnRecordPressed.AddUObject(this, &AWaveTrigger::SaveWaveInfo);
		EventManager::OnRecordReleased.AddUObject(this, &AWaveTrigger::FreezeWaves);
		EventManager::OnRewindFinished.AddUObject(this, &AWaveTrigger::LoadWaveInfo);
	}
}

void AWaveTrigger::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (!HasAuthority()) return;

	switch (currentState)
	{
	case EWaveTriggerState::DuringWave:
		CheckAllEnemiesDead();
		break;

	case EWaveTriggerState::BetweenWave:
		timeSinceWaveFinished += DeltaTime;

		if (timeSinceWaveFinished > TimeBetweenWaves)
		{
			StartNextWave();
		}
		break;
	}
}

void AWaveTrigger::StartFirstWave()
{
	if (HasAuthority())
	{
		currentWaveIndex = -1;

		if (GateBefore)
		{
			GateBefore->OnDoorFinishedClosing.Remove(closeDoorHandle);
		}

		EventManager::OnFightStarted.Broadcast();

		StartNextWave();
	}
}

void AWaveTrigger::StartNextWave()
{
	if (HasAuthority())
	{
		currentState = EWaveTriggerState::DuringWave;
		currentWaveIndex++;

		if (WaveInfo.Num() > currentWaveIndex)
		{
			for (FSpawnInfo spawnInfo : WaveInfo[currentWaveIndex].SpawnInfo)
			{
				if (spawnInfo.Nest)
				{
					spawnInfo.Nest->SpawnEnemies(spawnInfo.WorkerCount, spawnInfo.TankCount, spawnInfo.AssassinCount);
				}
			}

			RPC_PlaySpawnSound();
		}
	}
}

void AWaveTrigger::StopWave()
{
	if (HasAuthority())
	{
		if (currentWaveIndex == WaveInfo.Num() - 1)
		{
			currentState = EWaveTriggerState::Finished;

			if (GateAfter)
			{
				GateAfter->Open();
			}

			EventManager::OnFightEnded.Broadcast();
		}
		else
		{
			currentState = EWaveTriggerState::BetweenWave;
			timeSinceWaveFinished = 0.f;
		}
	}
}

void AWaveTrigger::CheckAllEnemiesDead()
{
	if (HasAuthority() && WaveInfo.Num() > currentWaveIndex)
	{
		bool allEnemiesDead = true;

		for (FSpawnInfo spawnInfo : WaveInfo[currentWaveIndex].SpawnInfo)
		{
			if (spawnInfo.Nest->RemainEnemies())
			{
				allEnemiesDead = false;
				break;
			}
		}

		if (allEnemiesDead)
		{
			StopWave();
		}
	}
}

void AWaveTrigger::BeginOverlap(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (!HasAuthority() || alreadyTriggered) return;

	if (OtherActor->IsA(AWarriorCharacter::StaticClass()))
	{
		warriorInside = true;
	}
	else if (OtherActor->IsA(AMageCharacter::StaticClass()))
	{
		mageInside = true;
	}
	else
	{
		return;
	}

	if ((warriorInside || !WarriorRequired) && (mageInside || !MageRequired))
	{
		if (GateAfter)
		{
			GateAfter->Close();
		}

		if(PillarGroup) {
			PillarGroup->ResetPillars();
		}

		if (GateBefore)
		{
			GateBefore->Close();
			closeDoorHandle = GateBefore->OnDoorFinishedClosing.AddUObject(this, &AWaveTrigger::StartFirstWave);
		}
		else
		{
			StartFirstWave();
		}

		alreadyTriggered = true;
	}
}

void AWaveTrigger::EndOverlap(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority() || alreadyTriggered) return;

	if (OtherActor->IsA(AWarriorCharacter::StaticClass()))
	{
		warriorInside = false;
	}
	else if (OtherActor->IsA(AMageCharacter::StaticClass()))
	{
		mageInside = false;
	}
}

void AWaveTrigger::ResetWaves()
{
	if (HasAuthority())
	{
		alreadyTriggered = false;
		warriorInside = false;
		mageInside = false;
		timeSinceWaveFinished = 0.f;
		currentWaveIndex = -1;
		currentState = EWaveTriggerState::NotStarted;
	}
}

void AWaveTrigger::FreezeWaves()
{
	if (HasAuthority())
	{
		currentState = EWaveTriggerState::Frozen;
	}
}

void AWaveTrigger::SaveWaveInfo(bool, float)
{
	if (HasAuthority())
	{
		savedInfo.warriorInside = warriorInside;
		savedInfo.mageInside = mageInside;
		savedInfo.alreadyTriggered = alreadyTriggered;
		savedInfo.closeDoorHandle = closeDoorHandle;
		savedInfo.timeSinceWaveFinished = timeSinceWaveFinished;
		savedInfo.currentWaveIndex = currentWaveIndex;
		savedInfo.currentState = currentState;
	}
}

void AWaveTrigger::LoadWaveInfo()
{
	if (HasAuthority())
	{
		warriorInside = savedInfo.warriorInside;
		mageInside = savedInfo.mageInside;
		alreadyTriggered = savedInfo.alreadyTriggered;
		closeDoorHandle = savedInfo.closeDoorHandle;
		timeSinceWaveFinished = savedInfo.timeSinceWaveFinished;
		currentState = savedInfo.currentState;

		if (savedInfo.currentWaveIndex == -1 && currentWaveIndex != -1)
		{
			StartFirstWave();	
		}
		else
		{
			currentWaveIndex = savedInfo.currentWaveIndex;
		}
	}
}

void AWaveTrigger::RPC_PlaySpawnSound_Implementation()
{
	PlaySpawnSound();

}