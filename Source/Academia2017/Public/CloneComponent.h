#pragma once

#include "Components/ActorComponent.h"
#include "SnapshotInfo.h"
#include "TimeEventManagerComponent.h"
#include "AttackHitbox.h"
#include "CloneComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACADEMIA2017_API UCloneComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCloneComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void Replay(const TArray<FSnapshotInfo> &snapshotData);
	
private:
	TArray<FSnapshotInfo> replayStack;
	TArray<FSnapshotInfo> rewindStack;
	bool replaying = false;
	bool recording = false;
	float relativeTime = 0;
	float relativeZero = 0;
	int stackIndex = 0;
	UTimeEventManagerComponent *timeEventManager;
	UAttackHitbox *AttackHitbox;

	void Rewind();
	void CacheNewTimeZero(bool isFixedDuration, float duration);
	void CancelRecord();
};
