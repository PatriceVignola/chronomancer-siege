// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Academia2017.h"
#include "Academia2017GameMode.h"
#include "EventManager.h"
#include "WarriorCharacter.h"
#include "MageCharacter.h"
#include "Engine.h"

void AAcademia2017GameMode::BeginPlay()
{
	Super::BeginPlay();

	EventManager::OnRecordPressed.AddUObject(this, &AAcademia2017GameMode::SendRecordStartedEvent);
	EventManager::OnRecordReleased.AddUObject(this, &AAcademia2017GameMode::SendRecordStoppedEvent);
	EventManager::OnCharacterDied.AddUObject(this, &AAcademia2017GameMode::SlowTimeForDeath);
	EventManager::OnDeathRewindFinished.AddUObject(this, &AAcademia2017GameMode::ResetTimeDilation);
	EventManager::OnMageSpawned.AddUObject(this, &AAcademia2017GameMode::StartFadingOut);

}

void AAcademia2017GameMode::BeginDestroy()
{
	Super::BeginDestroy();
}

void AAcademia2017GameMode::Tick( float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (characterDied && !deathRewindStarted)
	{
		timeSinceCharacterDied += DeltaTime;

		if (timeSinceCharacterDied >= DeathAnimationTime)
		{
			deathRewindStarted = true;
			EventManager::OnGameOver.Broadcast();
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
		}
	}
}

void AAcademia2017GameMode::SendRecordStartedEvent(bool isFixedDuration, float duration)
{
	OnRecordStarted.Broadcast();
}

void AAcademia2017GameMode::SendRecordStoppedEvent()
{
	OnRecordStopped.Broadcast();
}

void AAcademia2017GameMode::SlowTimeForDeath()
{
	if (!characterDied)
	{
		characterDied = true;
		timeSinceCharacterDied = 0.f;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);
		EventManager::OnDeathMusicLayerRequested.Broadcast();
	}
}

void AAcademia2017GameMode::ResetTimeDilation()
{
	characterDied = false;
	deathRewindStarted = false;
}

void AAcademia2017GameMode::PostLogin(APlayerController *playerController)
{
	Super::PostLogin(playerController);
	EventManager::OnPlayerJoined.Broadcast(playerController);
	SpawnPlayer(playerController);
}

void AAcademia2017GameMode::CachePlayerStarts()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), playerStarts);
}

void AAcademia2017GameMode::EnableInputs()
{
	for (APlayerController *playerController : playerControllers)
	{
		EnableInput(playerController);
	}
}

void AAcademia2017GameMode::StartFadingOut()
{
	if (GetWorld()->WorldType != EWorldType::PIE)
	{
		if (playerCount == playerStarts.Num())
		{
			OnStartFadingOut.Broadcast();
		}
	}
}

void AAcademia2017GameMode::SpawnPlayer(APlayerController *playerController)
{
	if (!playerController) return;

	if(playerStarts.Num() == 0)
	{
		CachePlayerStarts();
	}

	if (CharacterSpawnList.Num() > playerCount && playerStarts.Num() > playerCount)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		APawn *newPawn = GetWorld()->SpawnActor<APawn>(CharacterSpawnList[playerCount], playerStarts[playerCount]->GetTransform(), spawnParams);
		playerController->Possess(newPawn);

		playerCount++;

		if (playerCount == 2)
		{
			EventManager::OnLevelFinishedStreaming.Broadcast();
		}
	}

	EventManager::OnPlayerJoined.Broadcast(playerController);
}

void AAcademia2017GameMode::NotifyLevelStreamed(TArray<int32> characterIndexes)
{
	HasBeenLevelStreamed = true;

	if (playerStarts.Num() == 0)
	{
		CachePlayerStarts();
	}

	for (int i = 0; i < playerStarts.Num(); i++)
	{
		APlayerController *playerController = UGameplayStatics::GetPlayerController(GetWorld(), i);

		if (CharacterSpawnList.Num() > i && playerStarts.Num() > i && characterIndexes.Num() > i && characterIndexes[i] < CharacterSpawnList.Num())
		{
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			APawn *newPawn = GetWorld()->SpawnActor<APawn>(CharacterSpawnList[characterIndexes[i]], playerStarts[i]->GetTransform(), spawnParams);
			playerController->Possess(newPawn);
			DisableInput(playerController);
			playerControllers.Add(playerController);

			EventManager::OnPlayerJoined.Broadcast(playerController);
		}
	}

	EventManager::OnLevelFinishedStreaming.Broadcast();
	OnLevelFinishedStreaming.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("LEVEL FINISHED STREAMING!!!"));
}

void AAcademia2017GameMode::ReEnableInput()
{
	UE_LOG(LogTemp, Warning, TEXT("REENABLE INPUT!!!"));

	for (int i = 0; i < playerControllers.Num(); i++)
	{
		EnableInput(playerControllers[i]);
	}
}