#pragma once

#include "Components/ActorComponent.h"
#include "EnergyManagerComponent.h"
#include "RecorderComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACADEMIA2017_API URecorderComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Curve)
	UCurveFloat *RecordingCurve;

	UPROPERTY(EditDefaultsOnly)
	bool InstantCast = true;

	UPROPERTY(EditDefaultsOnly)
	float InstantCastDuration = 5.f;

	UPROPERTY(EditInstanceOnly)
	bool EnabledOnController = true;

	UPROPERTY(EditDefaultsOnly)
	float InstantCastCost = 50.f;

public:	
	URecorderComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartRecording();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopRecording();

	UPROPERTY(EditDefaultsOnly)
	float EnergyConsumptionSpeed = 20;

	UPROPERTY(EditDefaultsOnly)
	float MinimumRequiredEnergy = 10.f;

private:
	bool isRecording = false;
	bool isRewinding = false;

	void StartRecording();
	void StopRecording();
	void OnRewindFinished();
	void CancelRecording();
	void EnableRecording();
	
	float elapsedTime = 0.f;

	UInputComponent *inputComponent;
	UEnergyManagerComponent *energyManager;
};
