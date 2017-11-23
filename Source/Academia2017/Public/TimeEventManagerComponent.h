#pragma once

#include "Components/ActorComponent.h"
#include "SnapshotInfo.h"
#include "BoneSnapshotInfo.h"
#include "DeathSnapshotInfo.h"
#include "TimeEventManagerComponent.generated.h"

DECLARE_EVENT_TwoParams(EventManager, FRecordStoppedDelegate, const TArray<FSnapshotInfo> &, bool);

DECLARE_EVENT_OneParam(EventManager, FRewindFinishedDelegate, const TArray<FSnapshotInfo> &);
DECLARE_EVENT_OneParam(EventManager, FBoneRecordStoppedDelegate, const TArray<FBoneSnapshotInfo> &);
DECLARE_EVENT_OneParam(EventManager, FBoneRewindStoppedDelegate, const TArray<FBoneSnapshotInfo> &);
DECLARE_EVENT_OneParam(EventManager, FAttackStartedDelegate, int);

DECLARE_EVENT(EventManager, FReplayFinishedDelegate);
DECLARE_EVENT(EventManager, FCharacterDiedDelegate);
DECLARE_EVENT(EventManager, FCharacterRevivedDelegate);
DECLARE_EVENT(EventManager, FRecordStartedDelegate);

DECLARE_EVENT_OneParam(EventManager, FDeathRecordStoppedDelegate, const TArray<FDeathSnapshotInfo> &);
DECLARE_EVENT_OneParam(EventManager, FDeathBoneRecordStoppedDelegate, const TArray<FBoneSnapshotInfo> &);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACADEMIA2017_API UTimeEventManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTimeEventManagerComponent();

	FRecordStoppedDelegate OnRecordStopped;

	FRewindFinishedDelegate OnRewindFinished;
	FBoneRecordStoppedDelegate OnBoneRecordStopped;
	FBoneRewindStoppedDelegate OnBoneRewindStopped;
	FAttackStartedDelegate OnAttackStarted;

	FReplayFinishedDelegate OnReplayFinished;
	FCharacterDiedDelegate OnCharacterDied;
	FRecordStartedDelegate OnRecordStarted;

	FDeathRecordStoppedDelegate OnDeathRecordStopped;
	FDeathBoneRecordStoppedDelegate OnDeathBoneRecordStopped;
};
