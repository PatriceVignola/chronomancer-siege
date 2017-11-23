#include "Academia2017.h"
#include "EventManager.h"
#include "RecordInterupterComponent.h"

URecordInterupterComponent::URecordInterupterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URecordInterupterComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URecordInterupterComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// Executed only once
	if (!inputComponent && GetOwner()->InputComponent)
	{
		inputComponent = GetOwner()->InputComponent;
		inputComponent->BindAction("InteruptRecord", IE_Pressed, this, &URecordInterupterComponent::RequestRecordInterupt);
	}
}

void URecordInterupterComponent::RequestRecordInterupt()
{
	EventManager::OnRecordInteruptRequested.Broadcast();
}