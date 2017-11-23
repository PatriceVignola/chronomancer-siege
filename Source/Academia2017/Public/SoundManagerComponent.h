#pragma once

#include "Components/ActorComponent.h"
#include "SoundManagerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API USoundManagerComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	USoundManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Sound)
	void EnableMusic();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Sound)
	void PlayMainMenuMusic();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Sound)
	void PlayExplorationMusic();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void OnPlayDeathRewindLayerRequested();

	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void OnStopDeathRewindLayerRequested();

	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void OnRecordLayerRequested();

	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void OnRewindLayerRequested();

	UFUNCTION(BlueprintImplementableEvent, Category = Sound)
	void OnFightLayerRequested();

private:
	bool fighting = false;

	void OnRecordPressed(bool isFixedDuration, float duration);
	void OnRecordReleased();
	void OnRewindFinished();
	void OnFightStarted();
	void OnFightEnded();
	void OnDeathMusicLayerRequested();
	void OnDeathRewindFinished();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayRecordLayer();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayRewindLayer();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayMusicLayer();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayFightLayer();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayExplorationLayer();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PlayDeathRewindLayer();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StopDeathRewindLayer();
};
