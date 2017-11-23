// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "BossEgg.h"
#include "Net/UnrealNetwork.h"
#include "EggSpawningComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACADEMIA2017_API UEggSpawningComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEggSpawningComponent();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Replicated)
	bool IsActivated = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeBetweenEggSpawns = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ABossEgg> EggSpawnClass;
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpawnEgg();
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	float m_Timer = 0.0f;
	
};
