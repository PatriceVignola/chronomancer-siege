#pragma once

#include "DelegateCombinations.h"
#include "SnapshotInfo.h"
DECLARE_EVENT_TwoParams(EventManager, FWarriorHealthChanged, float, float);
DECLARE_EVENT_TwoParams(EventManager, FMageHealthChanged, float, float);
DECLARE_EVENT_TwoParams(EventManager, FRecordPressedDelegate, bool, float);
DECLARE_EVENT(EventManager, FRecordReleasedDelegate);
DECLARE_EVENT(EventManager, FRecordingLimitReached);
DECLARE_EVENT(EventManager, FGlobalRewindFinishedDelegate);
DECLARE_EVENT(EventManager, FGlobalDeathRewindFinishedDelegate);
DECLARE_EVENT(EventManager, FGameOverDelegate);
DECLARE_EVENT(EventManager, FGameOverRewindFinishedDelegate);
DECLARE_EVENT(EventManager, FFightStarted);
DECLARE_EVENT(EventManager, FFightEnded);
DECLARE_EVENT(EventManager, FCheckpointReachedDelegate);
DECLARE_EVENT(EventManager, FRecordInteruptRequestedDelegate);
DECLARE_EVENT(EventManager, FCharactersDiedDelegate);
DECLARE_EVENT(EventManager, FMageSpawnedDelegate);
DECLARE_EVENT(EventManager, FOnLevelFinishedStreaming);
DECLARE_EVENT(EventManager, FDeathMusicLayerRequested);
DECLARE_EVENT(EventManager, FRecordCancelledDelegate);
DECLARE_EVENT_OneParam(EventManager, FPlayerJoinedGameDelegate, APlayerController *);

class ACADEMIA2017_API EventManager
{
public:
	static FRecordPressedDelegate OnRecordPressed;
	static FRecordReleasedDelegate OnRecordReleased;
	static FRecordingLimitReached OnRecordingLimitReached;
	static FGlobalRewindFinishedDelegate OnRewindFinished;
	static FGlobalDeathRewindFinishedDelegate OnDeathRewindFinished;
	static FGameOverDelegate OnGameOver;
	static FFightStarted OnFightStarted;
	static FFightEnded OnFightEnded;
	static FCheckpointReachedDelegate OnCheckpointReached;
	static FRecordInteruptRequestedDelegate OnRecordInteruptRequested;
	static FCharactersDiedDelegate OnCharacterDied;
	static FWarriorHealthChanged OnWarriorChangedHealth;
	static FMageHealthChanged OnMageChangedHealth;
	static FPlayerJoinedGameDelegate OnPlayerJoined;
	static FMageSpawnedDelegate OnMageSpawned;
	static FDeathMusicLayerRequested OnDeathMusicLayerRequested;
	static FOnLevelFinishedStreaming OnLevelFinishedStreaming;
	static FRecordCancelledDelegate OnRecordCancelled;
};
