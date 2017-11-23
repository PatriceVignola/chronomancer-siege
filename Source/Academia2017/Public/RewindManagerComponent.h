#pragma once

#include "Components/ActorComponent.h"
#include "RewindManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API URewindManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat *RewindStartCurve = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat *RewindEndCurve = nullptr;

public:	
	URewindManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	float GetRewindDelta();
	bool IsRecording();
	float GetRelativeTime();

private:
	bool recording = false;
	bool rewinding = false;
	float rewindDelta = 0.f;
	float relativeTime = 0.f;
	float rewindStartCurveMinTime = 0.f;
	float rewindStartCurveMaxTime = 0.f;
	float rewindEndCurveMinTime = 0.f;

	void StartRecording(bool, float);
	void StartRewinding();
	void SetCurvesInfo();
	void UpdateRewindDelta(float DeltaTime);
	void OnRecordCancelled();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StartRecording();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StartRewinding();
};
