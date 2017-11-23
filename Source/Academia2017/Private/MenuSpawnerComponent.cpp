#include "Academia2017.h"
#include "Engine.h"
#include "MenuSpawnerComponent.h"

UMenuSpawnerComponent::UMenuSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMenuSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMenuSpawnerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}