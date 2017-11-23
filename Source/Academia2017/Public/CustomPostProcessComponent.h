#pragma once

#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Components/PostProcessComponent.h"
#include "CustomPostProcessComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPostProcessChangedDelegate, FPostProcessSettings, NewSettings);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UCustomPostProcessComponent : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LerpParameters, meta = (AllowPrivateAccess = "true"))
	bool RecordPostProcess = false;

	UPROPERTY(BlueprintAssignable)
	FPostProcessChangedDelegate OnPostProcessChanged;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FPostProcessSettings MaxPostProcessSettings;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPostProcessComponent *PostProcess;

public:	
	UCustomPostProcessComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	bool recordIsFixedDuration = false;
	float recordDuration = 0.f;
	bool isRecording = false;
	float lerpRatio = 0.f;
	FPostProcessSettings defaultPostProcessSettings;
	FPostProcessSettings currentPostProcessSettings;

	void StartLerping(bool isFixedDuration, float duration);
	void StopLerping();
	void OverrideBooleans();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StartLerping(bool isFixedDuration, float duration);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StopLerping();
};
