#include "Academia2017.h"
#include "RecordableBonesComponent.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "EventManager.h"

URecordableBonesComponent::URecordableBonesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URecordableBonesComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UActorComponent *comp = GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()))
	{
		mesh = Cast<USkeletalMeshComponent>(comp);
	}

	if(UActorComponent *comp = GetOwner()->FindComponentByClass(UTimeEventManagerComponent::StaticClass()))
	{
		timeEventManager = Cast<UTimeEventManagerComponent>(comp);

		if (GetOwner()->HasAuthority())
		{
			EventManager::OnRecordPressed.AddUObject(this, &URecordableBonesComponent::StartRecordingBones);
			EventManager::OnRecordReleased.AddUObject(this, &URecordableBonesComponent::StopRecordingBones);
			EventManager::OnGameOver.AddUObject(this, &URecordableBonesComponent::CancelRecordingBones);

			rewindManager = GetWorld()->GetGameState()->FindComponentByClass<URewindManagerComponent>();

			if (rewindManager && rewindManager->IsRecording())
			{
				RPC_StartRecordingBones();
				relativeTime = rewindManager->GetRelativeTime();
			}
		}

		if (timeEventManager)
		{
			timeEventManager->OnAttackStarted.AddUObject(this, &URecordableBonesComponent::RecordAttackSound);
		}
	}
}

void URecordableBonesComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (mesh && isRecording)
	{
		relativeTime += DeltaTime;
		recordStack.Add(FBoneSnapshotInfo(mesh->GetComponentSpaceTransforms(), relativeTime, frameAttackType, frameLeftFootstep, frameRightFootstep));
	}

	frameAttackType = -1;
	frameLeftFootstep = false;
	frameRightFootstep = false;
}

void URecordableBonesComponent::StartRecordingBones(bool isFixedDuration, float duration)
{
	RPC_StartRecordingBones();
}

void URecordableBonesComponent::StopRecordingBones()
{
	RPC_StopRecordingBones();
}

void URecordableBonesComponent::CancelRecordingBones()
{
	RPC_CancelRecordingBones();
}

void URecordableBonesComponent::RPC_StartRecordingBones_Implementation()
{
	isRecording = true;
	recordStack.Empty();
}

void URecordableBonesComponent::RPC_StopRecordingBones_Implementation()
{
	if (isRecording)
	{
		if (timeEventManager && mesh)
		{
			timeEventManager->OnBoneRecordStopped.Broadcast(recordStack);
		}

		isRecording = false;
		relativeTime = 0.f;
	}
}

void URecordableBonesComponent::RPC_CancelRecordingBones_Implementation()
{
	isRecording = false;
}

void URecordableBonesComponent::RecordAttackSound(int attackType)
{
	frameAttackType = attackType;
}

void URecordableBonesComponent::SaveLeftFootstepPlayed()
{
	frameLeftFootstep = true;
}

void URecordableBonesComponent::SaveRightFootstepPlayed()
{
	frameRightFootstep = true;
}