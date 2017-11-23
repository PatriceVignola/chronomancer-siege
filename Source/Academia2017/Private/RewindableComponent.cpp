#include "Academia2017.h"
#include "RewindableComponent.h"
#include "EventManager.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "Academia2017GameMode.h"

URewindableComponent::URewindableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void URewindableComponent::BeginPlay()
{
	Super::BeginPlay();

	if(UActorComponent *comp = GetOwner()->FindComponentByClass(UTimeEventManagerComponent::StaticClass()))
	{
		timeEventManager = Cast<UTimeEventManagerComponent>(comp);
		timeEventManager->OnRecordStopped.AddUObject(this, &URewindableComponent::Rewind);
	}

	if (UActorComponent *comp = GetOwner()->FindComponentByClass(UDamageableCharacterComponent::StaticClass()))
	{
		damageableComponent = Cast<UDamageableCharacterComponent>(comp);
	}

	rewindManager = GetWorld()->GetGameState()->FindComponentByClass<URewindManagerComponent>();

	if (GetOwner()->HasAuthority())
	{
		EventManager::OnCharacterDied.AddUObject(this, &URewindableComponent::CancelRewinding);
	}
}

void URewindableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (rewinding)
	{
		if(rewindStack.Num() == 0)
		{
			StopRewind();
			return;
		}

		float rewindSpeed = rewindManager ? rewindManager->GetRewindDelta() : DeltaTime;

		FSnapshotInfo info = rewindStack.Top();

		// If the player has a lower FPS than before, we may have to skip some info in the stack
		while(rewindStack.Num() > 0 && info.time >= relativeTime - rewindSpeed)
		{
			rewindStack.Pop();

			if(rewindStack.Num() > 0)
			{
				info = rewindStack.Top();
			}
		}

		// It's not really its job, but it will be a lot easier than duplicating the code
		replayStack.Add(info);

		float ratio = rewindSpeed / (relativeTime - info.time);

		if (damageableComponent)
		{
			damageableComponent->SetHealth(info.CurrentHealth);
		}

		GetOwner()->SetActorLocation(FMath::Lerp(GetOwner()->GetActorLocation(), info.pos, ratio));
		GetOwner()->SetActorRotation(FMath::Lerp(GetOwner()->GetActorRotation(), info.rot, ratio));

		relativeTime -= rewindSpeed;

		if(relativeTime <= 0)
		{
			StopRewind();
		}
	}
}
void URewindableComponent::Rewind(const TArray<FSnapshotInfo> &snapshotStack, bool destroyAfterRewind)
{
	if(snapshotStack.Num() > 0)
	{ 
		rewindStack = snapshotStack;
		rewinding = true;
		relativeTime = rewindStack.Top().time + GetWorld()->GetDeltaSeconds();

		// It's not really its job, but it will be a lot easier than duplicating the code
		replayStack.Add(rewindStack.Top());

		if (GetOwner()->HasAuthority())
		{
			shouldDestroyAfterRewind = destroyAfterRewind;
			GetOwner()->SetReplicateMovement(false);
			RPC_DisableInput();
		}

		OnEnableRewindPostProcessRequested.Broadcast();
	}
}

void URewindableComponent::StopRewind()
{
	if(GetOwner()->HasAuthority())
	{
		if (shouldDestroyAfterRewind)
		{
			GetOwner()->Destroy();
		}
		else
		{
			GetOwner()->SetReplicateMovement(true);

			if(timeEventManager)
			{
				timeEventManager->OnRewindFinished.Broadcast(replayStack);
			}

			RPC_StopRewind();
		}
	}
}

void URewindableComponent::RPC_StopRewind_Implementation()
{
	rewinding = false;

	if(APlayerController *controller = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		GetOwner()->EnableInput(controller);
	}

	relativeTime = 0;

	rewindStack.Empty();
	replayStack.Empty();

	OnDisableRewindPostProcessRequested.Broadcast();
}

void URewindableComponent::RPC_DisableInput_Implementation()
{
	if (APlayerController *controller = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		GetOwner()->DisableInput(controller);
	}
}

void URewindableComponent::CancelRewinding()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_CancelRewinding();
	}
}

void URewindableComponent::RPC_CancelRewinding_Implementation()
{
	rewinding = false;
	relativeTime = 0;
	rewindStack.Empty();
	replayStack.Empty();
	OnDisableRewindPostProcessRequested.Broadcast();
}