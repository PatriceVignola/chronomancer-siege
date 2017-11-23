// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ActivablePillar.h"
#include "Chronolith.h"
#include "PillarGroup.generated.h"

class AGate;
class AElevator;

UCLASS()
class ACADEMIA2017_API APillarGroup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APillarGroup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	float ResetDelay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	bool IsActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	bool IsActivated = false;

	UPROPERTY(EditInstanceOnly, Category = "Pillar")
	TArray<AActivablePillar*> Pillars;

	UPROPERTY(EditInstanceOnly, Category = "Pillar")
	AGate* Gate = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "Pillar")
	AElevator* Elevator = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "Pillar")
	AChronolith* Chronolith = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Pillar")
	void ResetPillars();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Pillar")
	void PlayCompletionSound();

private:
	float delay = 0.0f;
	bool m_wasGateActive = false;
	bool m_wasElevatorActive = false;
	bool savedState = false;

	UFUNCTION(NetMulticast, Reliable)
	void RPC_ResetPillars();
};

