// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "DestructibleComponent.h"
#include "FloorDestroyerComponent.h"


// Sets default values for this component's properties
UFloorDestroyerComponent::UFloorDestroyerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFloorDestroyerComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UFloorDestroyerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	TArray<FHitResult> hitResults;

	GetWorld()->LineTraceMultiByObjectType(hitResults,
										   GetComponentLocation(),
										   GetComponentLocation() - FVector(0, 0, TraceLength),
										   FCollisionObjectQueryParams(ECC_Destructible));

	for (FHitResult &hitResult : hitResults)
	{
		if (UDestructibleComponent *dm = Cast<UDestructibleComponent>(hitResult.GetComponent()))
		{
			TPair<FHitResult, float> pair;
			pair.Key = hitResult;
			pair.Value = 0.5f;

			pendingHitResults.Add(pair);
		}
	}

	for (int i = 0; i < pendingHitResults.Num(); i++)
	{
		TPair<FHitResult, float> &pair  = pendingHitResults[i];

		pair.Value -= DeltaTime;
		
		if (pair.Value <= 0)
		{
			if (UDestructibleComponent *dm = Cast<UDestructibleComponent>(pair.Key.GetComponent()))
			{
				dm->ApplyRadiusDamage(10, pair.Key.ImpactPoint, 300, 0, true);
				pendingHitResults.RemoveAt(i--);
			}
		}
	}
}

