// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CheatTeleporterComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UCheatTeleporterComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FVector> TeleportLocations;

public:	
	UCheatTeleporterComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	UInputComponent *inputComponent = nullptr;

	void TeleportToLocation1();
	void TeleportToLocation2();
	void TeleportToLocation3();
	void TeleportToLocation4();
	void TeleportToLocation5();
	void TeleportToLocation6();
	void TeleportToLocation7();
	void TeleportToLocation8();

	void TeleportToLocation(int index);

	UFUNCTION(Server, WithValidation, Reliable)
	void Server_RequestTeleportToLocation(int index);
};
