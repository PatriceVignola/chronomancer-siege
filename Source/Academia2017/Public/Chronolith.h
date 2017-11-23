// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Chronolith.generated.h"

UCLASS()
class ACADEMIA2017_API AChronolith : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChronolith();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent *Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UFUNCTION(BlueprintCallable, Category = Particle)
	void ActivateStunEffect();

	UFUNCTION(BlueprintImplementableEvent, Category = Particle)
	void BPActivateStunEffect();

private:

	UFUNCTION(NetMulticast, Reliable)
	void RPC_ActivateStunEffect();
};
