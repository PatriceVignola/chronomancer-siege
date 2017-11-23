#pragma once
#include "GameFramework/GameModeBase.h"
#include "PlayableCharacter.h"
#include "Academia2017GameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRecordStartedBlueprintDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRecordStoppedBlueprintDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartFadingOutDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlackOverlayRequestedDelegate);

// The characters spawn a few seconds after the level has finished streaming
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelVisuallyFinishedStreamingDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelFinishedStreamingDelegate);

UCLASS(minimalapi)
class AAcademia2017GameMode : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float DeathAnimationTime = 1.f;

	UPROPERTY(EditDefaultsOnly)
	float TimeDilation = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<APawn>> CharacterSpawnList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float FadeOutDuration = 3.f;

public:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick( float DeltaTime) override;
	virtual void PostLogin(APlayerController *playerController) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Multiplayer, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<ACharacter> FirstCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Multiplayer, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<ACharacter> SecondCharacterClass;

	UPROPERTY(BlueprintAssignable)
	FRecordStartedBlueprintDelegate OnRecordStarted;

	UPROPERTY(BlueprintAssignable)
	FRecordStoppedBlueprintDelegate OnRecordStopped;

	UPROPERTY(BlueprintAssignable)
	FStartFadingOutDelegate OnStartFadingOut;

	UPROPERTY(BlueprintAssignable)
	FBlackOverlayRequestedDelegate OnBlackOverlayRequested;

	UPROPERTY(BlueprintAssignable)
	FLevelFinishedStreamingDelegate OnLevelFinishedStreaming;

	UFUNCTION(BlueprintCallable, Category = GameMode)
	void EnableInputs();

	UFUNCTION(BlueprintCallable, Category = LevelStreaming)
	void NotifyLevelStreamed(TArray<int32> characterIndexes);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = LevelStreaming)
	FLevelVisuallyFinishedStreamingDelegate OnLevelVisuallyFinishedStreaming;

	UPROPERTY(BlueprintReadOnly, Category = LevelStreaming)
	bool HasBeenLevelStreamed = false;

	UFUNCTION(BlueprintCallable, Category = LevelStreaming)
	void ReEnableInput();

private:
	float timeSinceCharacterDied = 0.f;
	bool characterDied = false;
	bool deathRewindStarted = false;
	TArray<AActor *> playerStarts;

	UPROPERTY()
	TArray<APlayerController *> playerControllers;
	int playerCount = 0;

	void SendRecordStartedEvent(bool isFixedDuration, float duration);
	void SendRecordStoppedEvent();
	void SlowTimeForDeath();
	void ResetTimeDilation();
	void CachePlayerStarts();
	void StartFadingOut();
	void SpawnPlayer(APlayerController *playerController);
};



