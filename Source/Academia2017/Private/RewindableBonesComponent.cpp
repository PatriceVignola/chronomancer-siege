#include "Academia2017.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "EventManager.h"
#include "RewindableBonesComponent.h"

URewindableBonesComponent::URewindableBonesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URewindableBonesComponent::BeginPlay()
{
	Super::BeginPlay();

	mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();

	if (mesh)
	{
		CacheDynamicMaterials();

		poseableMesh = NewObject<UPoseableMeshComponent>(GetOwner(), UPoseableMeshComponent::StaticClass(), "RewindablePoseableMesh");
		poseableMesh->RegisterComponent();
		poseableMesh->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
		poseableMesh->SetSkeletalMesh(mesh->SkeletalMesh);
		poseableMesh->SetWorldLocation(mesh->GetComponentLocation());
		poseableMesh->SetWorldRotation(mesh->GetComponentRotation());
		poseableMesh->SetWorldScale3D(mesh->GetComponentScale());

		for (int i = 0; i < mesh->GetNumMaterials(); i++)
		{
			poseableMesh->SetMaterial(i, mesh->GetMaterial(i));
		}

		poseableMesh->SetHiddenInGame(true, true);
	}

	timeEventManager = GetOwner()->FindComponentByClass<UTimeEventManagerComponent>();

	if(timeEventManager)
	{
		timeEventManager->OnBoneRecordStopped.AddUObject(this, &URewindableBonesComponent::StartRewindingBones);

		if (GetOwner()->HasAuthority())
		{
			EventManager::OnRewindFinished.AddUObject(this, &URewindableBonesComponent::StopRewindingBones);
		}
	}

	rewindManager = GetWorld()->GetGameState()->FindComponentByClass<URewindManagerComponent>();

	if (GetOwner()->HasAuthority())
	{
		EventManager::OnCharacterDied.AddUObject(this, &URewindableBonesComponent::CancelRewindingBones);
	}
}

void URewindableBonesComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (mesh && isRewinding && rewindStack.Num() > 0)
	{
		if (pendingSetMeshHidden && mesh->bRecentlyRendered)
		{
			SetMeshVisibility(false);
			poseableMesh->SetHiddenInGame(false, true);
			pendingSetMeshHidden = false;
		}

		float rewindSpeed = rewindManager ? rewindManager->GetRewindDelta() : DeltaTime;

		FBoneSnapshotInfo info = rewindStack.Top();

		// If the player has a lower FPS than before, we may have to skip some info in the stack
		while(rewindStack.Num() > 0 && info.time >= relativeTime - rewindSpeed)
		{
			rewindStack.Pop();

			if(rewindStack.Num() > 0)
			{
				info = rewindStack.Top();
			}
		}

		replayStack.Add(info);

		float ratio = rewindSpeed / (relativeTime - info.time);

		if (!pendingSetMeshHidden && poseableMesh)
		{
			for (int i = 0; i < info.transforms.Num(); i++)
			{
				FName boneName = poseableMesh->GetBoneName(i);
				FTransform &boneTransform = info.transforms[i];
				/*
				FVector pos = FMath::Lerp(poseableMesh->GetBoneLocationByName(boneName, EBoneSpaces::ComponentSpace), boneTransform.GetTranslation(), ratio);
				FQuat quat = FMath::Lerp(poseableMesh->GetBoneRotationByName(boneName, EBoneSpaces::ComponentSpace).Quaternion(), boneTransform.GetRotation(), ratio);
				poseableMesh->SetBoneLocationByName(boneName, pos, EBoneSpaces::ComponentSpace);
				poseableMesh->SetBoneRotationByName(boneName, quat.Rotator(), EBoneSpaces::ComponentSpace);*/

				/*poseableMesh->SetBoneLocationByName(boneName, boneTransform.GetTranslation(), EBoneSpaces::ComponentSpace);
				poseableMesh->SetBoneRotationByName(boneName, boneTransform.GetRotation().Rotator(), EBoneSpaces::ComponentSpace);*/

				poseableMesh->SetBoneTransformByName(boneName, boneTransform, EBoneSpaces::ComponentSpace);
			}
		}

		relativeTime -= rewindSpeed;
	}
}

void URewindableBonesComponent::StartRewindingBones(const TArray<FBoneSnapshotInfo> &recordStack)
{
	rewindStack = recordStack;
	isRewinding = true;

	// Since setting hidden to false always sets bRecentlyRendered to true, we only want to do it if the actor
	// is visible
	if (mesh->bRecentlyRendered || poseableMesh->bRecentlyRendered || GetOwner()->GetComponentByClass(UCloneComponent::StaticClass()))
	{
		SetMeshVisibility(false);
		poseableMesh->SetHiddenInGame(false, true);
	}
	else
	{
		pendingSetMeshHidden = true;
	}

	if (rewindStack.Num() > 0)
	{
		relativeTime = rewindStack.Top().time + GetWorld()->GetDeltaSeconds();
	}
}

void URewindableBonesComponent::StopRewindingBones()
{
	RPC_StopRewindingBones();
}

void URewindableBonesComponent::RPC_StopRewindingBones_Implementation()
{
	if (poseableMesh && mesh)
	{
		poseableMesh->SetHiddenInGame(true, true);
		SetMeshVisibility(true);
	}
	
	isRewinding = false;
	pendingSetMeshHidden = false;

	if (timeEventManager)
	{
		timeEventManager->OnBoneRewindStopped.Broadcast(replayStack);
	}

	rewindStack.Empty();
	replayStack.Empty();
}

void URewindableBonesComponent::CancelRewindingBones()
{
	if (GetOwner()->HasAuthority())
	{
		RPC_CancelRewindingBones();
	}
}

void URewindableBonesComponent::RPC_CancelRewindingBones_Implementation()
{
	isRewinding = false;
	pendingSetMeshHidden = false;
	rewindStack.Empty();
	replayStack.Empty();
}

void URewindableBonesComponent::SetMeshVisibility(bool visible)
{
	if (!mesh) return;

	if (GetOwner()->IsA(AWarriorCharacter::StaticClass()) && InvisibleMaterial)
	{
		int materialsCount = mesh->GetNumMaterials();

		if (visible)
		{
			for (int i = 0; i < materialsCount; i++)
			{
				mesh->SetMaterial(i, dynamicMaterials[i]);
			}
		}
		else
		{
			for (int i = 0; i < materialsCount; i++)
			{
				mesh->SetMaterial(i, InvisibleMaterial);
			}
		}
	}
	else
	{
		mesh->SetHiddenInGame(!visible, true);
	}
}

void URewindableBonesComponent::CacheDynamicMaterials()
{
	if (mesh && GetOwner()->IsA(AWarriorCharacter::StaticClass()))
	{
		int materialsCount = mesh->GetNumMaterials();

		for (int i = 0; i < materialsCount; i++)
		{
			UMaterialInstanceDynamic *dynamicMaterial = mesh->CreateDynamicMaterialInstance(i);
			dynamicMaterials.Add(dynamicMaterial);
		}
	}
}