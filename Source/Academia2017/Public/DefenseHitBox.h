// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HitBoxComponent.h"
#include "DefenseHitBox.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Collision), meta = (BlueprintSpawnableComponent))
class ACADEMIA2017_API UDefenseHitBox : public UHitBoxComponent
{
	GENERATED_BODY()
public:
	UDefenseHitBox();
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	
	
};
