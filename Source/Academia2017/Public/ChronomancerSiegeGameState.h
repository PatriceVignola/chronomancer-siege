#pragma once

#include "GameFramework/GameStateBase.h"
#include "ChronomancerSiegeGameState.generated.h"

UCLASS()
class ACADEMIA2017_API AChronomancerSiegeGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AChronomancerSiegeGameState();
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = PostProcess)
	void EnableRecordPostProcess();

	UFUNCTION(BlueprintImplementableEvent, Category = PostProcess)
	void DisableRecordPostProcess();

	UFUNCTION(BlueprintImplementableEvent, Category = PostProcess)
	void EnableRewindPostProcess();

	UFUNCTION(BlueprintImplementableEvent, Category = PostProcess)
	void DisableRewindPostProcess();

private:
	void OnRecordPressed(bool, float);
	void OnRecordReleased();
	void OnRewindFinished();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_OnRecordPressed();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_OnRecordReleased();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_OnRewindFinished();
};
