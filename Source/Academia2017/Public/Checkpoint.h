#pragma once

#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class ACADEMIA2017_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent *Collider;

public:
	ACheckpoint();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = CheckpointHeal)
	void RechargePlayers();

private:
	bool active = true;
	bool m_triggered = false;

	UFUNCTION()
	void ResetDeathRecording(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);


	UFUNCTION(Server, Reliable, WithValidation)
	void RPC_RechargePlayers();

	void Private_RechargePlayers();
	
};
