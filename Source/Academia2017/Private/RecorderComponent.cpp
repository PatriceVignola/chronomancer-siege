#include "Academia2017.h"
#include "RecorderComponent.h"
#include "EventManager.h"

URecorderComponent::URecorderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bReplicates = true;
}

void URecorderComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		EventManager::OnRecordInteruptRequested.AddUObject(this, &URecorderComponent::StopRecording);
		EventManager::OnRewindFinished.AddUObject(this, &URecorderComponent::OnRewindFinished);
		EventManager::OnCharacterDied.AddUObject(this, &URecorderComponent::CancelRecording);
		EventManager::OnDeathRewindFinished.AddUObject(this, &URecorderComponent::EnableRecording);

		if (UActorComponent *comp = GetOwner()->FindComponentByClass(UEnergyManagerComponent::StaticClass()))
		{
			energyManager = Cast<UEnergyManagerComponent>(comp);
		}
	}
}

void URecorderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Executed only once
	if (!inputComponent && GetOwner()->InputComponent)
	{
		inputComponent = GetOwner()->InputComponent;

		inputComponent->BindAction(EnabledOnController ? "Record" : "Record_Debug", IE_Pressed, this, &URecorderComponent::StartRecording);

		if (!InstantCast)
		{
			inputComponent->BindAction(EnabledOnController ? "Record" : "Record_Debug", IE_Released, this, &URecorderComponent::StopRecording);
		}
	}

	if (GetOwner()->HasAuthority() && isRecording)
	{
		if (InstantCast && elapsedTime >= InstantCastDuration)
		{
			StopRecording();
		}

		elapsedTime += DeltaTime;
	}
}

void URecorderComponent::StartRecording()
{
	if(GetOwner()->HasAuthority())
	{
		if (!isRecording && !isRewinding)
		{
			if (energyManager)
			{
				if (energyManager->GetRemainingEnergy() >= MinimumRequiredEnergy)
				{
					if (InstantCast && energyManager->GetRemainingEnergy() >= InstantCastDuration * EnergyConsumptionSpeed)
					{
						energyManager->ConsumeEnergy(InstantCastCost);
					}

					isRecording = true;
					elapsedTime = 0.f;
					EventManager::OnRecordPressed.Broadcast(InstantCast, InstantCastDuration);
				}
			}
			else
			{
				isRecording = true;
				elapsedTime = 0.f;
				EventManager::OnRecordPressed.Broadcast(InstantCast, InstantCastDuration);
			}
		}
	}
	else
	{
		Server_StartRecording();
	}
}

void URecorderComponent::StopRecording()
{
	if(GetOwner()->HasAuthority())
	{
		if (isRecording)
		{
			isRecording = false;
			isRewinding = true;
			EventManager::OnRecordReleased.Broadcast();
		}
	}
	else
	{
		Server_StopRecording();
	}
}

void URecorderComponent::Server_StartRecording_Implementation()
{
	if (GetOwner()->HasAuthority())
	{
		StartRecording();
	}
}

bool URecorderComponent::Server_StartRecording_Validate()
{
	return true;
}

void URecorderComponent::Server_StopRecording_Implementation()
{
	if (GetOwner()->HasAuthority())
	{
		StopRecording();
	}
}

bool URecorderComponent::Server_StopRecording_Validate()
{
	return true;
}

void URecorderComponent::OnRewindFinished()
{
	isRewinding = false;
}

void URecorderComponent::CancelRecording()
{
	isRewinding = true;
	isRecording = false;
	EventManager::OnRecordCancelled.Broadcast();
}

void URecorderComponent::EnableRecording()
{
	isRewinding = false;
}