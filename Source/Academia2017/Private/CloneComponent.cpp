#include "Academia2017.h"
#include "EventManager.h"
#include "CloneComponent.h"

UCloneComponent::UCloneComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCloneComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UActorComponent *comp = GetOwner()->FindComponentByClass(UTimeEventManagerComponent::StaticClass()))
	{
		timeEventManager = Cast<UTimeEventManagerComponent>(comp);
		EventManager::OnRecordPressed.AddUObject(this, &UCloneComponent::CacheNewTimeZero);
		EventManager::OnRecordReleased.AddUObject(this, &UCloneComponent::Rewind);
		EventManager::OnGameOver.AddUObject(this, &UCloneComponent::CancelRecord);
		timeEventManager->OnRewindFinished.AddUObject(this, &UCloneComponent::Replay);
	}

	if (UActorComponent *comp = GetOwner()->FindComponentByClass(UAttackHitbox::StaticClass()))
	{
		AttackHitbox = Cast<UAttackHitbox>(comp);
	}
}

void UCloneComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (GetOwner()->HasAuthority() && replaying)
	{
		if(replayStack.Num() == 0)
		{
			replaying = false;

			return;
		}

		FSnapshotInfo info = replayStack.Top();

		if (AttackHitbox)
		{
			AttackHitbox->CanDamage = info.CanDamage;
		}
			
		// If the player has a lower FPS than before, we may have to skip some info in the stack
		while(replayStack.Num() > 0 && info.time < relativeTime + DeltaTime)
		{
			if (recording)
			{
				info.time -= relativeZero;
				rewindStack.Add(info);
			}

			replayStack.Pop();

			if(replayStack.Num() > 0)
			{
				info = replayStack.Top();
			}
		}

		float ratio = DeltaTime / (info.time - relativeTime);

		FVector newPos = FMath::Lerp(GetOwner()->GetActorLocation(), info.pos, ratio);
		FRotator newRot = FMath::Lerp(GetOwner()->GetActorRotation(), info.rot, ratio);

		GetOwner()->SetActorRotation(newRot);
		GetOwner()->SetActorLocation(newPos);

		relativeTime += DeltaTime;
	}
}

void UCloneComponent::CacheNewTimeZero(bool isFixedDuration, float duration)
{
	recording = true;
	stackIndex = replayStack.Num() - 1;
	relativeZero = relativeTime;
	rewindStack.Empty();
}

void UCloneComponent::Rewind()
{
	if (recording)
	{
		recording = false;
		replaying = false;
		timeEventManager->OnRecordStopped.Broadcast(rewindStack, false);
	}
}

void UCloneComponent::Replay(const TArray<FSnapshotInfo> &snapshotData)
{
	for (FSnapshotInfo &snapshotInfo : replayStack)
	{
		snapshotInfo.time -= relativeZero;
	}

	for (FSnapshotInfo snapshotInfo : snapshotData)
	{
		replayStack.Add(snapshotInfo);
	}

	replaying = true;
	relativeTime = 0;

	stackIndex = replayStack.Num() - 1;
}

void UCloneComponent::CancelRecord()
{
	if (GetOwner()->HasAuthority())
	{
		recording = false;
	}
}