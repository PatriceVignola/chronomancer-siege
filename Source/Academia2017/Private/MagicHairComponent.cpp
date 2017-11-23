#include "Academia2017.h"
#include "MagicHairComponent.h"
#include "EventManager.h"
#include "TimeEventManagerComponent.h"

UMagicHairComponent::UMagicHairComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMagicHairComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UTimeEventManagerComponent *timeEventManager = GetOwner()->FindComponentByClass<UTimeEventManagerComponent>())
	{
		EventManager::OnRecordPressed.AddUObject(this, &UMagicHairComponent::DeactivateHairPhysics);
		timeEventManager->OnRewindFinished.AddUObject(this, &UMagicHairComponent::ReactivateHairPhysics);
	}

	mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
}

void UMagicHairComponent::DeactivateHairPhysics(bool isFixedDuration, float duration)
{
	if (mesh)
	{
		mesh->bDisableClothSimulation = true;
	}
}

void UMagicHairComponent::ReactivateHairPhysics(const TArray<FSnapshotInfo> &)
{
	if (mesh)
	{
		mesh->bDisableClothSimulation = false;
	}
}