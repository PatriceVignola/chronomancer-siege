#include "Academia2017.h"
#include "RewindableComponent.h"
#include "Net/UnrealNetwork.h"
#include "EventManager.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "DisappearAfterRewindComponent.h"

UDisappearAfterRewindComponent::UDisappearAfterRewindComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bReplicates = true;
}

void UDisappearAfterRewindComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		if(UActorComponent *comp = GetOwner()->FindComponentByClass(UTimeEventManagerComponent::StaticClass()))
		{
			timeEventManager = Cast<UTimeEventManagerComponent>(comp);
			EventManager::OnRecordPressed.AddUObject(this, &UDisappearAfterRewindComponent::NoticeRecordingStarted);
			EventManager::OnRecordReleased.AddUObject(this, &UDisappearAfterRewindComponent::NoticeRecordingStopped);
			EventManager::OnGameOver.AddUObject(this, &UDisappearAfterRewindComponent::CancelRecord);
			timeEventManager->OnRewindFinished.AddUObject(this, &UDisappearAfterRewindComponent::NoticeRewindFinished);
			timeEventManager->OnReplayFinished.AddUObject(this, &UDisappearAfterRewindComponent::FadeOut);

			rewindManager = GetWorld()->GetGameState()->FindComponentByClass<URewindManagerComponent>();
		}
	}

	CacheDynamicMaterials();
}

void UDisappearAfterRewindComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDisappearAfterRewindComponent, timeSinceReplayFinished);
}

void UDisappearAfterRewindComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (GetOwner()->HasAuthority())
	{
		if (replayFinished)
		{
			if (isRewinding)
			{
				timeSinceReplayFinished -= rewindManager ? rewindManager->GetRewindDelta() : DeltaTime;

				if (timeSinceReplayFinished <= 0)
				{
					replayFinished = false;
					timeSinceReplayFinished = 0.f;
				}
			}
			else
			{
				timeSinceReplayFinished += DeltaTime;

				if (!isRecording && timeSinceReplayFinished / fadingDuration >= 1.f)
				{
					GetOwner()->Destroy();
				}
			}

			OnTimeStampReplicated();
		}
	}
}

void UDisappearAfterRewindComponent::FadeOut()
{
	if (GetOwner()->HasAuthority() && !replayFinished)
	{
		replayFinished = true;
		timeSinceReplayFinished = 0;
		RPC_PlayDisappearingSound();
	}
}

void UDisappearAfterRewindComponent::CacheDynamicMaterials()
{
	TArray<UActorComponent *> primitives = GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass());

	for (UActorComponent *comp : primitives)
	{
		UPrimitiveComponent *primitive = Cast<UPrimitiveComponent>(comp);

		int materialsCount = primitive->GetNumMaterials();

		for (int i = 0; i < materialsCount; i++)
		{
			UMaterialInstanceDynamic *dynamicMaterial = primitive->CreateDynamicMaterialInstance(i);
			primitive->SetMaterial(i, dynamicMaterial);
			dynamicMaterials.Add(dynamicMaterial);
		}
	}
}

void UDisappearAfterRewindComponent::SetMaterialsAlpha(float newAlpha)
{
	for (UMaterialInstanceDynamic *dynamicMaterial : dynamicMaterials)
	{
		if (dynamicMaterial)
		{
			dynamicMaterial->SetScalarParameterValue("Alpha", newAlpha);
		}
	}
}

void UDisappearAfterRewindComponent::NoticeRecordingStarted(bool isFixedDuration, float duration)
{
	isRecording = true;
}

void UDisappearAfterRewindComponent::NoticeRecordingStopped()
{
	if (isRecording)
	{
		isRecording = false;
		isRewinding = true;
	}
}

void UDisappearAfterRewindComponent::NoticeRewindFinished(const TArray<FSnapshotInfo> &)
{
	isRewinding = false;
}

void UDisappearAfterRewindComponent::OnTimeStampReplicated()
{
	SetMaterialsAlpha(FMath::Lerp(1.f, 0.f, FMath::Clamp(timeSinceReplayFinished / fadingDuration, 0.f, 1.f)));
}

void UDisappearAfterRewindComponent::CancelRecord()
{
	isRecording = false;
}

void UDisappearAfterRewindComponent::RPC_PlayDisappearingSound_Implementation()
{
	PlayDisappearingSound();
}