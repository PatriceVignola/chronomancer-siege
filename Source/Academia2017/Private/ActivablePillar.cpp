#include "Academia2017.h"
#include "ActivablePillar.h"

AActivablePillar::AActivablePillar()
{
	PrimaryActorTick.bCanEverTick = true;

	BodyHitBox = CreateDefaultSubobject<UActivableHitbox>(TEXT("BodyHitBox"));
	BodyHitBox->SetupAttachment(RootComponent);

	bReplicates = true;
}

void AActivablePillar::BeginPlay()
{
	Super::BeginPlay();

	State = PillarState::Deactivated;
}

void AActivablePillar::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if(HasAuthority()) {
		if(BodyHitBox->IsActivated && State != PillarState::Activated) {
			RPC_PillarActivated();
			State = PillarState::Activated;
		}
	}
}

void AActivablePillar::Activate()
{
	delay = 0.0f;
	if(HasAuthority()) {
		if(BodyHitBox->IsActivated && State != PillarState::Activated) {
			RPC_PillarActivated();
			State = PillarState::Activated;
		}
	}
}

void AActivablePillar::Deactivate() {
	if(HasAuthority()) {
		BodyHitBox->IsActivated = false;
		RPC_PillarDesactivated();
		State = PillarState::Deactivated;
	}
}

void AActivablePillar::Active() {
	if(HasAuthority()) {
		RPC_PillarActive();
		State = PillarState::Active;
	}
}

void AActivablePillar::RPC_PillarActivated_Implementation()
{
	PillarActivated();
}

void AActivablePillar::RPC_PillarDesactivated_Implementation()
{
	PillarDesactivated();
}

void AActivablePillar::RPC_PillarActive_Implementation() 
{
	PillarActive();
}