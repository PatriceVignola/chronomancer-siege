// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"

#include "AIBucketComponent.h"


// Sets default values for this component's properties
UAIBucketComponent::UAIBucketComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UAIBucketComponent::BeginPlay()
{

}

// Called every frame
void UAIBucketComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
}

// Called when the game starts
void UAIBucketComponent::UnregisterBucket()
{
	if (registeredBucket != nullptr)
	{
		registeredBucket->UnregisterBucket(BucketValue);
		registeredBucket = nullptr;
	}
}
void UAIBucketComponent::RegisterBucket(UPlayerBucketComponent *PlayerBucketComponent)
{
	if (registeredBucket != PlayerBucketComponent && registeredBucket != nullptr)
	{
		UnregisterBucket();
	}
	PlayerBucketComponent->RegisterBucket(BucketValue);
	registeredBucket = PlayerBucketComponent;
}


