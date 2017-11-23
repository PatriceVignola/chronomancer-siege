#pragma once

#include "Components/ActorComponent.h"
#include "DeathRewindManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOverlayFadeTicked, float, Alpha);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UDeathRewindManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat *RewindStartCurve = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat *RewindEndCurve = nullptr;

public:	
	UDeathRewindManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	float GetRewindDelta();
	float GetRelativeTime();
	bool IsRecording();
	bool IsRewinding();

	UPROPERTY(BlueprintAssignable)
	FOverlayFadeTicked OnOverlayFadeTicked;

private:
	bool recording = false;
	bool rewinding = false;
	float rewindDelta = 0.f;
	float relativeTime = 0.f;
	float overlayRelativeTime = 0.f;
	float rewindStartCurveMinTime = 0.f;
	float rewindStartCurveMaxTime = 0.f;
	float rewindEndCurveMinTime = 0.f;

	void StartRecording();
	void StartRewinding();
	void SetCurvesInfo();
	void UpdateRewindDelta(float DeltaTime);
	void TickOverlayFade();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StartRecording();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StartRewinding();
};
