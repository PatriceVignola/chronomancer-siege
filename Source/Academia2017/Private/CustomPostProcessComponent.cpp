#include "Academia2017.h"
#include "EventManager.h"
#include "CustomPostProcessComponent.h"

UCustomPostProcessComponent::UCustomPostProcessComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCustomPostProcessComponent::BeginPlay()
{
	Super::BeginPlay();
	
	PostProcess = NewObject<UPostProcessComponent>(GetOwner());
	PostProcess->RegisterComponent();
	PostProcess->AttachToComponent(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));

	if (GetOwner()->HasAuthority() && RecordPostProcess)
	{
		EventManager::OnRecordPressed.AddUObject(this, &UCustomPostProcessComponent::StartLerping);
		EventManager::OnRecordReleased.AddUObject(this, &UCustomPostProcessComponent::StopLerping);
	}

	OverrideBooleans();
}

void UCustomPostProcessComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (recordIsFixedDuration && isRecording)
	{
		lerpRatio = FMath::Clamp(lerpRatio + DeltaTime / recordDuration, 0.f, 1.f);

		currentPostProcessSettings.ColorSaturation = FMath::Lerp(MaxPostProcessSettings.ColorSaturation, defaultPostProcessSettings.ColorSaturation, lerpRatio);
		currentPostProcessSettings.ColorContrast = FMath::Lerp(MaxPostProcessSettings.ColorContrast, defaultPostProcessSettings.ColorContrast, lerpRatio);
		currentPostProcessSettings.ColorGain = FMath::Lerp(MaxPostProcessSettings.ColorGain, defaultPostProcessSettings.ColorGain, lerpRatio);
		currentPostProcessSettings.WhiteTint = FMath::Lerp(MaxPostProcessSettings.WhiteTint, defaultPostProcessSettings.WhiteTint, lerpRatio);
		currentPostProcessSettings.SceneFringeIntensity = FMath::Lerp(MaxPostProcessSettings.SceneFringeIntensity, defaultPostProcessSettings.SceneFringeIntensity, lerpRatio);
		currentPostProcessSettings.BloomThreshold = FMath::Lerp(MaxPostProcessSettings.BloomThreshold, defaultPostProcessSettings.BloomThreshold, lerpRatio);
		currentPostProcessSettings.LensFlareIntensity = FMath::Lerp(MaxPostProcessSettings.LensFlareIntensity, defaultPostProcessSettings.LensFlareIntensity, lerpRatio);
		currentPostProcessSettings.LensFlareTint = FMath::Lerp(MaxPostProcessSettings.LensFlareTint, defaultPostProcessSettings.LensFlareTint, lerpRatio);
		currentPostProcessSettings.VignetteIntensity = FMath::Lerp(MaxPostProcessSettings.VignetteIntensity, defaultPostProcessSettings.VignetteIntensity, lerpRatio);
		currentPostProcessSettings.DepthOfFieldFocalRegion = FMath::Lerp(MaxPostProcessSettings.DepthOfFieldFocalRegion, defaultPostProcessSettings.DepthOfFieldFocalRegion, lerpRatio);
		currentPostProcessSettings.MotionBlurPerObjectSize = FMath::Lerp(MaxPostProcessSettings.MotionBlurPerObjectSize, defaultPostProcessSettings.MotionBlurPerObjectSize, lerpRatio);

		OnPostProcessChanged.Broadcast(currentPostProcessSettings);
	}
}

void UCustomPostProcessComponent::StartLerping(bool isFixedDuration, float duration)
{
	if (GetOwner()->HasAuthority())
	{
		RPC_StartLerping(isFixedDuration, duration);
	}
}

void UCustomPostProcessComponent::StopLerping()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_StopLerping();
	}
}

void UCustomPostProcessComponent::OverrideBooleans()
{
	currentPostProcessSettings.bOverride_ColorSaturation = true;
	currentPostProcessSettings.bOverride_ColorContrast = true;
	currentPostProcessSettings.bOverride_ColorGain = true;
	currentPostProcessSettings.bOverride_WhiteTint = true;
	currentPostProcessSettings.bOverride_SceneFringeIntensity = true;
	currentPostProcessSettings.bOverride_BloomThreshold = true;
	currentPostProcessSettings.bOverride_LensFlareIntensity = true;
	currentPostProcessSettings.bOverride_LensFlareTint = true;
	currentPostProcessSettings.bOverride_VignetteIntensity = true;
	currentPostProcessSettings.bOverride_DepthOfFieldFocalRegion = true;
	currentPostProcessSettings.bOverride_MotionBlurPerObjectSize = true;
}

void UCustomPostProcessComponent::RPC_StartLerping_Implementation(bool isFixedDuration, float duration)
{
	PostProcess->bEnabled = true;
	recordIsFixedDuration = isFixedDuration;
	recordDuration = duration;
	isRecording = true;
	lerpRatio = 0.f;
}

void UCustomPostProcessComponent::RPC_StopLerping_Implementation()
{
	PostProcess->bEnabled = false;
	isRecording = false;
}