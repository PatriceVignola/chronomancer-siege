#include "Academia2017.h"
#include "EventManager.h"
#include "DeathRewindManagerComponent.h"

UDeathRewindManagerComponent::UDeathRewindManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void UDeathRewindManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		EventManager::OnGameOver.AddUObject(this, &UDeathRewindManagerComponent::StartRewinding);
		EventManager::OnCheckpointReached.AddUObject(this, &UDeathRewindManagerComponent::StartRecording);
	}
}

void UDeathRewindManagerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (rewinding || overlayRelativeTime > 0.f)
	{
		UpdateRewindDelta(DeltaTime);
	}

	if (recording)
	{
		relativeTime += DeltaTime;
	}
	else if (rewinding)
	{
		relativeTime -= rewindDelta;

		// We skip over the timestamps we won't use
		if (relativeTime < rewindStartCurveMaxTime && relativeTime > rewindEndCurveMinTime)
		{
			relativeTime = rewindEndCurveMinTime;
		}

		if (GetOwner()->HasAuthority() && relativeTime <= 0.f)
		{
			StartRecording();
			EventManager::OnDeathRewindFinished.Broadcast();
		}
	}

	if (overlayRelativeTime > 0.f)
	{
		overlayRelativeTime -= rewindDelta;

		// We skip over the timestamps we won't use
		if (overlayRelativeTime < rewindStartCurveMaxTime && overlayRelativeTime > rewindEndCurveMinTime)
		{
			overlayRelativeTime = rewindEndCurveMinTime;
		}

		TickOverlayFade();
	}
}

void UDeathRewindManagerComponent::StartRecording()
{
	RPC_StartRecording();
}

void UDeathRewindManagerComponent::RPC_StartRecording_Implementation()
{
	relativeTime = 0.f;
	rewinding = false;
	recording = true;
}

void UDeathRewindManagerComponent::StartRewinding()
{
	RPC_StartRewinding();
}

void UDeathRewindManagerComponent::RPC_StartRewinding_Implementation()
{
	overlayRelativeTime = relativeTime;
	recording = false;
	rewinding = true;
	SetCurvesInfo();
}

float UDeathRewindManagerComponent::GetRewindDelta()
{
	return rewindDelta;
}

void UDeathRewindManagerComponent::SetCurvesInfo()
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

void UDeathRewindManagerComponent::UpdateRewindDelta(float DeltaTime)
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

float UDeathRewindManagerComponent::GetRelativeTime()
{
	return relativeTime;
}

bool UDeathRewindManagerComponent::IsRecording()
{
	return recording;
}

bool UDeathRewindManagerComponent::IsRewinding()
{
	return rewinding;
}

void UDeathRewindManagerComponent::TickOverlayFade()
{
	float alpha;

	if (overlayRelativeTime > rewindStartCurveMaxTime)
	{
		// We are fading in
		alpha = 1 - (overlayRelativeTime - rewindStartCurveMaxTime) / (rewindStartCurveMinTime - rewindStartCurveMaxTime);
	}
	else
	{
		// We are fading out
		alpha = overlayRelativeTime / rewindEndCurveMinTime;
	}

	OnOverlayFadeTicked.Broadcast(alpha);
}