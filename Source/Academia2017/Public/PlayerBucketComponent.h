// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "PlayerBucketComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACADEMIA2017_API UPlayerBucketComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerBucketComponent();
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MaxBucketValue = 100;
	int CurrentBucketValue = 0;
	// Called when the game starts
	void RegisterBucket(int BucketValue);
	void UnregisterBucket(int BucketValue);
	bool IsFull(int BucketValue);

		
	
};
