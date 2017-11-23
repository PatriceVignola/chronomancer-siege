#pragma once

#include "GameFramework/Actor.h"
#include "ActivatedByPillarBase.h"
#include "PlayableCharacter.h"
#include "EventManager.h"
#include "Elevator.generated.h"

UCLASS()
class ACADEMIA2017_API AElevator : public AActivatedByPillarBase
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent *TriggerZone;

	UPROPERTY(VisibleAnywhere)
	USceneComponent *TriggerZoneContainer;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent *InvisibleWall;

public:	
	AElevator();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = Elevator)
	void SetUpAnimationFinished();

	UFUNCTION(BlueprintCallable, Category = Elevator)
	void SetDownAnimationFinished();

private:
	int peopleInsideCount = 0;
	bool upAnimationFinished = false;
	bool hasBeenActivated = false;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_SetUpAnimationFinished();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_SetDownAnimationFinished();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_ActivateElevator();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_ForceElevatorRewind();
};
