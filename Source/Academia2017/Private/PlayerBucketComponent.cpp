// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "PlayerBucketComponent.h"


// Sets default values for this component's properties
UPlayerBucketComponent::UPlayerBucketComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}
// Called when the game starts
 void UPlayerBucketComponent::BeginPlay() 
{

}

// Called every frame
 void UPlayerBucketComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) 
{
}


void UPlayerBucketComponent::RegisterBucket(int BucketValue)
{
	CurrentBucketValue += BucketValue;
}
void UPlayerBucketComponent::UnregisterBucket(int BucketValue)
{
	CurrentBucketValue -= BucketValue;
}
bool UPlayerBucketComponent::IsFull(int BucketValue)
{
	return  CurrentBucketValue + BucketValue > MaxBucketValue;
}