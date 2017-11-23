#include "Academia2017.h"
#include "EventManager.h"
#include "Elevator.h"
#include "ActivatedByPillarBase.h"

AActivatedByPillarBase::AActivatedByPillarBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	bReplicates = true;
}

void AActivatedByPillarBase::BeginPlay()
{
	Super::BeginPlay();

	if (InitiallyOpened)
	{
		OnOpenActivableRequested.Broadcast();
		opened = true;
	}

	if (HasAuthority())
	{
		// TEMPORARY, UNTIL WE BUILD A BETTER SYSTEM
		EventManager::OnCheckpointReached.AddUObject(this, &AActivatedByPillarBase::SaveActivableState);
		EventManager::OnGameOver.AddUObject(this, &AActivatedByPillarBase::LoadActivableState);
		EventManager::OnDeathRewindFinished.AddUObject(this, &AActivatedByPillarBase::ResetAnimScale);
	}
}

void AActivatedByPillarBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AActivatedByPillarBase::Open()
{
	if (HasAuthority())
	{	
		RPC_Open();
	}
}

void AActivatedByPillarBase::Close()
{
	if (HasAuthority() && opened)
	{
		RPC_Close();
	}
}

void AActivatedByPillarBase::SetActive() 
{
	if(HasAuthority()) 
	{
		RPC_SetActive();
	}
}

const bool AActivatedByPillarBase::isOpenableByPillar() const {
	return OpenableByPillar;
}

void AActivatedByPillarBase::SetDeactivated() {
	if(HasAuthority()) {
		RPC_SetDeactivated();
	}
}

void AActivatedByPillarBase::RPC_Open_Implementation()
{
	OnPillarActivated.Broadcast();
	OnOpenActivableRequested.Broadcast();
	opened = true;
}

void AActivatedByPillarBase::RPC_Close_Implementation()
{
	OnPillarDeactivated.Broadcast();
	OnCloseActivableRequested.Broadcast();
	opened = false;
}

void AActivatedByPillarBase::RPC_SetActive_Implementation() 
{
	OnPillarActive.Broadcast();
}

void AActivatedByPillarBase::SaveActivableState()
{
	savedGameState = opened;
}

void AActivatedByPillarBase::LoadActivableState()
{
	RPC_LoadActivableState();
}

void AActivatedByPillarBase::RPC_LoadActivableState_Implementation()
{
	if (IsA(AElevator::StaticClass())) return;

	Mesh->GlobalAnimRateScale = 100.f;

	if (savedGameState)
	{
		Open();
	}
	else
	{
		Close();
	}
}

void AActivatedByPillarBase::ResetAnimScale()
{
	RPC_ResetAnimScale();
}

void AActivatedByPillarBase::RPC_ResetAnimScale_Implementation()
{
	Mesh->GlobalAnimRateScale = 1.f;
}

void AActivatedByPillarBase::RPC_SetDeactivated_Implementation() {
	OnPillarDeactivated.Broadcast();
}