 #include "Academia2017.h"
#include "EventManager.h"
#include "RewindManagerComponent.h"

URewindManagerComponent::URewindManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void URewindManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		EventManager::OnRecordPressed.AddUObject(this, &URewindManagerComponent::StartRecording);
		EventManager::OnRecordReleased.AddUObject(this, &URewindManagerComponent::StartRewinding);
		EventManager::OnRecordCancelled.AddUObject(this, &URewindManagerComponent::OnRecordCancelled);
	}
}

void URewindManagerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (recording)
	{
		relativeTime += DeltaTime;
	}
	else if (rewinding)
	{
		UpdateRewindDelta(DeltaTime);

		relativeTime -= rewindDelta;

		if (relativeTime <= 0.f)
		{
			rewinding = false;

			if (GetOwner()->HasAuthority())
			{
				EventManager::OnRewindFinished.Broadcast();
			}
		}
	}
}

void URewindManagerComponent::StartRecording(bool, float)
{
	if (GetOwner()->HasAuthority())
	{
		RPC_StartRecording();
	}
}

void URewindManagerComponent::StartRewinding()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_StartRewinding();
	}
}

void URewindManagerComponent::RPC_StartRecording_Implementation()
{
	relativeTime = 0.f;
	recording = true;
}

void URewindManagerComponent::RPC_StartRewinding_Implementation()
{
	recording = false;
	rewinding = true;
	SetCurvesInfo();
}

float URewindManagerComponent::GetRewindDelta()
{
	return rewindDelta;
}

void URewindManagerComponent::SetCurvesInfo()
{
	rewindStartCurveMinTime = relativeTime;
	rewindStartCurveMaxTime = relativeTime;
	rewindEndCurveMinTime = 0.f;

	if (RewindStartCurve)
	{
		float minTime, maxTime;
		RewindStartCurve->GetTimeRange(minTime, maxTime);
		rewindStartCurveMaxTime = rewindStartCurveMinTime - maxTime;
	}

	if (RewindEndCurve)
	{
		float minTime, maxTime;
		RewindEndCurve->GetTimeRange(minTime, maxTime);
		rewindEndCurveMinTime = maxTime;
	}
}

void URewindManagerComponent::UpdateRewindDelta(float DeltaTime)
{
	rewindDelta = DeltaTime;

	if (RewindEndCurve && relativeTime < rewindEndCurveMinTime)
	{
		rewindDelta *= RewindEndCurve->GetFloatValue(rewindEndCurveMinTime - relativeTime);
		
	}
	else if (RewindStartCurve)
	{
		rewindDelta *= RewindStartCurve->GetFloatValue(rewindStartCurveMinTime - relativeTime);
	}
}

bool URewindManagerComponent::IsRecording()
{
	return recording;
}

float URewindManagerComponent::GetRelativeTime()
{
	return relativeTime;
}

void URewindManagerComponent::OnRecordCancelled()
{
	recording = false;
}