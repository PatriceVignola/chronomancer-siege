#include "Academia2017.h"
#include "DrawDebugHelpers.h"
#include "FeetIKComponent.h"

UFeetIKComponent::UFeetIKComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFeetIKComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UActorComponent *comp = GetOwner()->GetComponentByClass(USkinnedMeshComponent::StaticClass()))
	{
		mesh = Cast<USkeletalMeshComponent>(comp);
	}
}

void UFeetIKComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	FVector leftFootPos = mesh->GetBoneLocation(LeftFootName, EBoneSpaces::WorldSpace);
	FVector leftFootCompPos = mesh->GetBoneLocation(LeftFootName, EBoneSpaces::ComponentSpace);

	FVector origin, extent;
	float radius;
	UKismetSystemLibrary::GetComponentBounds(GetOwner()->GetRootComponent(), origin, extent, radius);

	FVector leftFootTraceStart(leftFootPos.X, leftFootPos.Y, origin.Z);
	FVector leftFootTraceEnd(leftFootPos.X, leftFootPos.Y, origin.Z - extent.Z);
	FHitResult leftFootResult;
	//GetWorld()->LineTraceSingleByObjectType(leftFootResult, leftFootTraceStart, leftFootTraceEnd, FCollisionObjectQueryParams(ECC_Visibility));
	GetWorld()->LineTraceSingleByChannel(leftFootResult, leftFootTraceStart, leftFootTraceEnd, ECollisionChannel::ECC_Visibility);
	LeftFootEndEffector = leftFootResult.GetActor() ? leftFootTraceStart.Z - leftFootTraceEnd.Z - leftFootResult.Distance : 0;

	//DrawDebugLine(GetWorld(), leftFootTraceStart, leftFootTraceEnd, FColor::Red, false, -1.0f, 0, 5);


	FVector rightFootPos = mesh->GetBoneLocation(RightFootName, EBoneSpaces::WorldSpace);

	FVector rightFootTraceStart(rightFootPos.X, rightFootPos.Y, origin.Z);
	FVector rightFootTraceEnd(rightFootPos.X, rightFootPos.Y, origin.Z - extent.Z);
	FHitResult rightFootResult;
	GetWorld()->LineTraceSingleByChannel(rightFootResult, rightFootTraceStart, rightFootTraceEnd, ECollisionChannel::ECC_Visibility);
	RightFootEndEffector = rightFootResult.GetActor() ? rightFootTraceStart.Z - rightFootTraceEnd.Z - rightFootResult.Distance : 0;

	//DrawDebugLine(GetWorld(), rightFootTraceStart, rightFootTraceEnd, FColor::Red, false, -1.0f, 0, 5);
}