#include "Academia2017.h"
#include "EventManager.h"
#include "RewindableBonesComponent.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "ReplayableBonesComponent.h"

UReplayableBonesComponent::UReplayableBonesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UReplayableBonesComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UActorComponent *comp = GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()))
	{
		mesh = Cast<USkeletalMeshComponent>(comp);

		poseableMesh = NewObject<UPoseableMeshComponent>(GetOwner(), UPoseableMeshComponent::StaticClass(), "ReplayablePoseableMesh");
		poseableMesh->RegisterComponent();
		poseableMesh->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
		poseableMesh->SetSkeletalMesh(mesh->SkeletalMesh);
		poseableMesh->SetWorldLocation(mesh->GetComponentLocation());
		poseableMesh->SetWorldRotation(mesh->GetComponentRotation());
		TArray<UActorComponent*> CompByTags = GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), FName("OffsetHitbox"));

		if (CompByTags.Num() > 0)
		{
			attackHitbox = Cast<USceneComponent>(CompByTags[0]);

			if (attackHitbox)
			{
				attackHitbox->AttachToComponent(poseableMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), SocketName);
			}
		}

		for (int i = 0; i < mesh->GetNumMaterials(); i++)
		{
			poseableMesh->SetMaterial(i, mesh->GetMaterial(i));
		}

		poseableMesh->SetHiddenInGame(true, true);
	}

	timeEventManager = GetOwner()->FindComponentByClass<UTimeEventManagerComponent>();

	if (timeEventManager)
	{
		timeEventManager->OnBoneRewindStopped.AddUObject(this, &UReplayableBonesComponent::StartReplayingBones);
	}

	if (GetOwner()->HasAuthority())
	{
		EventManager::OnRecordPressed.AddUObject(this, &UReplayableBonesComponent::CacheNewTimeZero);
		EventManager::OnRecordReleased.AddUObject(this, &UReplayableBonesComponent::NoticeRecordStopped);
	}
}

void UReplayableBonesComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (replaying)
	{
		if(stackIndex < 0)
		{
			replaying = false;

			if (timeEventManager)
			{
				timeEventManager->OnReplayFinished.Broadcast();
			}

			// We stop the animation to disable any potential damage
			if (mesh)
			{
				mesh->GetAnimInstance()->Montage_Stop(0);
			}

			return;
		}

		FBoneSnapshotInfo info = replayStack[stackIndex];

		// If the player has a lower FPS than before, we may have to skip some info in the stack
		while(stackIndex >= 0 && info.time < timeSinceReplayStarted + DeltaTime)
		{
			if (info.attackType != -1)
			{
				OnCloneAttackStarted.Broadcast(info.attackType);
			}

			if (info.leftFootstep)
			{
				OnCloneLeftFootstepPlayed.Broadcast();
			}

			if (info.rightFootstep)
			{
				OnCloneRightFootstepPlayed.Broadcast();
			}

			if (!recording)
			{
				replayStack.Pop();
			}
			else
			{
				rewindStack.Add(info);
			}

			stackIndex--;

			if(stackIndex >= 0)
			{
				info = replayStack[stackIndex];
			}
		}

		float ratio = DeltaTime / (info.time - timeSinceReplayStarted);

		for (int i = 0; i < info.transforms.Num(); i++)
		{
			FName boneName = poseableMesh->GetBoneName(i);
			FTransform &boneTransform = info.transforms[i];

			/*FVector pos = FMath::Lerp(poseableMesh->GetBoneLocationByName(boneName, EBoneSpaces::ComponentSpace), boneTransform.GetTranslation(), ratio);
			FQuat quat = FMath::Lerp(poseableMesh->GetBoneRotationByName(boneName, EBoneSpaces::ComponentSpace).Quaternion(), boneTransform.GetRotation(), ratio);

			poseableMesh->SetBoneLocationByName(boneName, boneTransform.GetTranslation(), EBoneSpaces::ComponentSpace);
			poseableMesh->SetBoneRotationByName(boneName, boneTransform.GetRotation().Rotator(), EBoneSpaces::ComponentSpace);*/

			poseableMesh->SetBoneTransformByName(boneName, boneTransform, EBoneSpaces::ComponentSpace);
		}

		timeSinceReplayStarted += DeltaTime;
	}
	else if (recording && rewindStack.Num() > 0)
	{
		FBoneSnapshotInfo info = rewindStack.Top();
		info.time += DeltaTime;
		rewindStack.Add(info);
	}
}

void UReplayableBonesComponent::CacheInitialRecordStack(const TArray<FBoneSnapshotInfo> &recordStack)
{
	replayStack.Empty();

	for (int i = recordStack.Num() - 1; i >= 0; i--)
	{
		replayStack.Add(recordStack[i]);
	}

	StartReplayingBones(replayStack);
}

void UReplayableBonesComponent::CacheNewTimeZero(bool isFixedDuration, float duration)
{
	if (GetOwner()->HasAuthority())
	{
		RPC_CacheNewTimeZero();
	}
}

void UReplayableBonesComponent::RPC_CacheNewTimeZero_Implementation()
{
	recording = true;

	stackIndex = replayStack.Num() - 1;

	for (int i = 0; i < replayStack.Num(); i++)
	{
		replayStack[i].time -= timeSinceReplayStarted;
	}

	timeSinceReplayStarted = 0.f;
}

void UReplayableBonesComponent::NoticeRecordStopped()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_NoticeRecordStopped();
	}
}

void UReplayableBonesComponent::RPC_NoticeRecordStopped_Implementation()
{
	recording = false;
	replaying = false;

	if (poseableMesh)
	{
		poseableMesh->SetHiddenInGame(true, true);
	}

	// We need to send at least the most recent pose
	if (rewindStack.Num() == 0)
	{
		TArray<FName> boneNames;
		poseableMesh->GetBoneNames(boneNames);

		TArray<FTransform> transforms;

		for (FName boneName : boneNames)
		{
			FTransform recent(poseableMesh->GetBoneQuaternion(boneName, EBoneSpaces::ComponentSpace),
							  poseableMesh->GetBoneLocationByName(boneName, EBoneSpaces::ComponentSpace));

			transforms.Add(recent);
		}

		rewindStack.Add(FBoneSnapshotInfo(transforms, timeSinceReplayStarted, -1, false, false));
	}

	timeEventManager->OnBoneRecordStopped.Broadcast(rewindStack);
	rewindStack.Empty();
}

void UReplayableBonesComponent::StartReplayingBones(const TArray<FBoneSnapshotInfo> &snapshotData)
{
	replaying = true;
	timeSinceReplayStarted = 0.f;
	stackIndex = replayStack.Num() - 1;

	if (mesh)
	{
		mesh->SetHiddenInGame(true, true);
	}
	
	if (poseableMesh)
	{
		poseableMesh->SetHiddenInGame(false, true);
	}
}