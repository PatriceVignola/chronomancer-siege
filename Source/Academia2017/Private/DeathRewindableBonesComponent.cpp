#include "Academia2017.h"
#include "DeathRewindableBonesComponent.h"
#include "EventManager.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "WarriorCharacter.h"
#include "Academia2017GameMode.h"

UDeathRewindableBonesComponent::UDeathRewindableBonesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void UDeathRewindableBonesComponent::BeginPlay()
{
	Super::BeginPlay();

	mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();

	if (mesh)
	{
		CacheDynamicMaterials();

		poseableMesh = NewObject<UPoseableMeshComponent>(GetOwner(), UPoseableMeshComponent::StaticClass(), "DeathRewindablePoseableMesh");
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

	if(UActorComponent *comp = GetOwner()->FindComponentByClass(UTimeEventManagerComponent::StaticClass()))
	{
		timeEventManager = Cast<UTimeEventManagerComponent>(comp);
		timeEventManager->OnDeathBoneRecordStopped.AddUObject(this, &UDeathRewindableBonesComponent::Rewind);
	}

	rewindManager = GetWorld()->GetGameState()->FindComponentByClass<UDeathRewindManagerComponent>();

	if (GetOwner()->HasAuthority())
	{
		EventManager::OnDeathRewindFinished.AddUObject(this, &UDeathRewindableBonesComponent::StopRewind);
	}
}

void UDeathRewindableBonesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (rewindManager && IsRewinding() && rewindStack.Num() > 0)
	{
		float rewindSpeed = rewindManager ? rewindManager->GetRewindDelta() : DeltaTime;

		FBoneSnapshotInfo info = rewindStack.Top();

		// If the player has a lower FPS than before, we may have to skip some info in the stack
		while(rewindStack.Num() > 0 && info.time >= rewindManager->GetRelativeTime() - rewindSpeed)
		{
			rewindStack.Pop();

			if(rewindStack.Num() > 0)
			{
				info = rewindStack.Top();
			}
		}

		if (poseableMesh)
		{
			for (int i = 0; i < info.transforms.Num(); i++)
			{
				FName boneName = poseableMesh->GetBoneName(i);
				FTransform &boneTransform = info.transforms[i];
				poseableMesh->SetBoneTransformByName(boneName, boneTransform, EBoneSpaces::ComponentSpace);
			}
		}
	}
}
void UDeathRewindableBonesComponent::Rewind(const TArray<FBoneSnapshotInfo> &snapshotStack)
{
	if(snapshotStack.Num() > 0)
	{
		rewindStack = snapshotStack;

		if (mesh && poseableMesh)
		{
			poseableMesh->SetHiddenInGame(false, true);
			SetMeshVisibility(false);
		}
	}
}

void UDeathRewindableBonesComponent::StopRewind()
{
	if(GetOwner()->HasAuthority())
	{
		GetOwner()->SetReplicateMovement(true);

		RPC_StopRewind();
	}
}

void UDeathRewindableBonesComponent::RPC_StopRewind_Implementation()
{
	rewindStack.Empty();

	if (mesh && poseableMesh)
	{
		poseableMesh->SetHiddenInGame(true, true);
		SetMeshVisibility(true);
	}
}

bool UDeathRewindableBonesComponent::IsRewinding()
{
	return rewindManager ? rewindManager->IsRewinding() : false;
}

void UDeathRewindableBonesComponent::SetMeshVisibility(bool visible)
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

void UDeathRewindableBonesComponent::CacheDynamicMaterials()
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