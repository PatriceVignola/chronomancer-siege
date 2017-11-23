#include "Academia2017.h"
#include "Elevator.h"
#include "CloneComponent.h"

AElevator::AElevator()
{
	PrimaryActorTick.bCanEverTick = true;

	TriggerZoneContainer = CreateDefaultSubobject<USceneComponent>(TEXT("TriggerZoneContainer"));
	TriggerZoneContainer->SetupAttachment(Mesh, "MoveableFloorSocket");

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	TriggerZone->SetupAttachment(TriggerZoneContainer);

	InvisibleWall = CreateDefaultSubobject<UBoxComponent>(TEXT("InvisibleWall"));
	InvisibleWall->SetupAttachment(RootComponent);
}

void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &AElevator::OnBeginOverlap);
		TriggerZone->OnComponentEndOverlap.AddDynamic(this, &AElevator::OnEndOverlap);
	}
}

void AElevator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AElevator::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		if (OtherActor->IsA(APlayableCharacter::StaticClass()) && !OtherActor->FindComponentByClass<UCloneComponent>())
		{
			peopleInsideCount++;

			if (!hasBeenActivated && peopleInsideCount >= 2)
			{
				RPC_ActivateElevator();
			}
		}
	}
}

void AElevator::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		if (OtherActor->IsA(APlayableCharacter::StaticClass()) && !OtherActor->FindComponentByClass<UCloneComponent>())
		{
			peopleInsideCount--;

			if (hasBeenActivated && !upAnimationFinished && peopleInsideCount < 2)
			{
				RPC_ForceElevatorRewind();
			}
		}
	}
}

void AElevator::RPC_ActivateElevator_Implementation()
{
	OnCloseActivableRequested.Broadcast();
	hasBeenActivated = true;
	Mesh->GlobalAnimRateScale = 1.f;
}

void AElevator::SetUpAnimationFinished()
{
	if (HasAuthority())
	{
		RPC_SetUpAnimationFinished();
	}
}

void AElevator::SetDownAnimationFinished()
{
	if (HasAuthority())
	{
		RPC_SetDownAnimationFinished();
	}
}

void AElevator::RPC_SetUpAnimationFinished_Implementation()
{
	InvisibleWall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	upAnimationFinished = true; 
}

void AElevator::RPC_SetDownAnimationFinished_Implementation()
{
	InvisibleWall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	upAnimationFinished = false;
}

void AElevator::RPC_ForceElevatorRewind_Implementation()
{
	Mesh->GlobalAnimRateScale = -1.f;
	hasBeenActivated = false;
}