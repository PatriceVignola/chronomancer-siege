// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/BoxComponent.h"
#include "HitBoxComponent.generated.h"
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EHitboxType : uint8
{
	NONE 		UMETA(DisplayName = "None"),
	DAMAGABLE 	UMETA(DisplayName = "Damagable"),
	DEFENSE 	UMETA(DisplayName = "Defense"),
	ATTACK 		UMETA(DisplayName = "Attack"),
	ACTIVATABLE UMETA(DisplayName = "Activatable")
};
UCLASS(ClassGroup = (Collision), meta = (BlueprintSpawnableComponent))
class ACADEMIA2017_API UHitBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UHitBoxComponent();
	EHitboxType HitBoxType;


		
	
};
