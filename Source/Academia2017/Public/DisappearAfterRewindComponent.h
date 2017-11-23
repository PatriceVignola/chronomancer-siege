#pragma once

#include "Components/ActorComponent.h"
#include "TimeEventManagerComponent.h"
#include "SnapshotInfo.h"
#include "RewindManagerComponent.h"
#include "DisappearAfterRewindComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class ACADEMIA2017_API UDisappearAfterRewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDisappearAfterRewindComponent();
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(EditDefaultsOnly)
	float fadingDuration = 1.f;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Sounds)
	void PlayDisappearingSound();

private:
	TArray<UMaterialInstanceDynamic *> dynamicMaterials;
	UTimeEventManagerComponent *timeEventManager = nullptr;
	URewindManagerComponent *rewindManager = nullptr;

	void FadeOut();
	
	void CacheDynamicMaterials();
	void SetMaterialsAlpha(float newAlpha);
	bool isRecording = false, isRewinding = false;

	UPROPERTY(ReplicatedUsing = OnTimeStampReplicated)
	float timeSinceReplayFinished = 0.f;

	float replayFinishedTimeStamp = 0.f;
	bool replayFinished = false;

	void NoticeRecordingStarted(bool isFixedDuration, float duration);
	void NoticeRecordingStopped();
	void NoticeRewindFinished(const TArray<FSnapshotInfo> &);

	UFUNCTION()
	void OnTimeStampReplicated();
	
	void CancelRecord();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayDisappearingSound();
};
