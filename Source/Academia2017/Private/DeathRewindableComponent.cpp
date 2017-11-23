#include "Academia2017.h"
#include "DeathRewindableComponent.h"
#include "EventManager.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "Academia2017GameMode.h"

UDeathRewindableComponent::UDeathRewindableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void UDeathRewindableComponent::BeginPlay()
{
	Super::BeginPlay();

	if(UActorComponent *comp = GetOwner()->FindComponentByClass(UTimeEventManagerComponent::StaticClass()))
	{
		timeEventManager = Cast<UTimeEventManagerComponent>(comp);
		timeEventManager->OnDeathRecordStopped.AddUObject(this, &UDeathRewindableComponent::Rewind);
	}

	if (UActorComponent *comp = GetOwner()->FindComponentByClass(UDamageableCharacterComponent::StaticClass()))
	{
		damageableComponent = Cast<UDamageableCharacterComponent>(comp);
	}

	rewindManager = GetWorld()->GetGameState()->FindComponentByClass<UDeathRewindManagerComponent>();

	if (GetOwner()->HasAuthority())
	{
		EventManager::OnDeathRewindFinished.AddUObject(this, &UDeathRewindableComponent::StopRewind);

		if (rewindManager && rewindManager->IsRecording())
		{
			destroyAfterRewind = true;
		}
	}
}

void UDeathRewindableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (rewindManager && IsRewinding() && rewindStack.Num() > 0)
	{
		float rewindSpeed = rewindManager ? rewindManager->GetRewindDelta() : DeltaTime;

		FDeathSnapshotInfo info = rewindStack.Top();

		// If the player has a lower FPS than before, we may have to skip some info in the stack
		while(rewindStack.Num() > 0 && info.time >= rewindManager->GetRelativeTime() - rewindSpeed)
		{
			rewindStack.Pop();

			if(rewindStack.Num() > 0)
			{
				info = rewindStack.Top();
			}
		}

		float ratio = rewindSpeed / (rewindManager->GetRelativeTime() - info.time);

		if (damageableComponent)
		{
			damageableComponent->SetHealth(info.CurrentHealth);
		}

		GetOwner()->SetActorLocation(FMath::Lerp(GetOwner()->GetActorLocation(), info.pos, ratio));
		GetOwner()->SetActorRotation(FMath::Lerp(GetOwner()->GetActorRotation(), info.rot, ratio));

		if(destroyAfterRewind && rewindStack.Num() == 0 && GetOwner()->HasAuthority())
		{
			GetOwner()->Destroy();
		}
	}
}
void UDeathRewindableComponent::Rewind(const TArray<FDeathSnapshotInfo> &snapshotStack)
{
	if(snapshotStack.Num() > 0)
	{
		rewindStack = snapshotStack;

		if(APlayerController *controller = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
		{
			GetOwner()->DisableInput(controller);
		}

		GetOwner()->SetReplicateMovement(false);
	}
}

void UDeathRewindableComponent::StopRewind()
{
	if(GetOwner()->HasAuthority())
	{
		if (destroyAfterRewind)
		{
			GetOwner()->Destroy();
		}
		else
		{
			GetOwner()->SetReplicateMovement(true);
			RPC_StopRewind();
		}
	}
}

void UDeathRewindableComponent::RPC_StopRewind_Implementation()
{
	if(APlayerController *controller = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		GetOwner()->EnableInput(controller);
	}

	rewindStack.Empty();
}

void UDeathRewindableComponent::RPC_DisableInput_Implementation()
{
	if(APlayerController *controller = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		GetOwner()->DisableInput(controller);
	}
}

void UDeathRewindableComponent::RPC_EnableInput_Implementation()
{
	if(APlayerController *controller = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		GetOwner()->EnableInput(controller);
	}
}

bool UDeathRewindableComponent::IsRewinding()
{
	return rewindManager ? rewindManager->IsRewinding() : false;
}