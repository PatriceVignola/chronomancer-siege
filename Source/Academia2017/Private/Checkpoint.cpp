#include "Academia2017.h"
#include "EventManager.h"
#include "PlayableCharacter.h"
#include "DamageableCharacterComponent.h"
#include "Engine.h"
#include "WarriorCharacter.h"
#include "MageCharacter.h"
#include "EnergyManagerComponent.h"
#include "Checkpoint.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	RootComponent = Collider;
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Collider->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::ResetDeathRecording);
	}
}

void ACheckpoint::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ACheckpoint::ResetDeathRecording(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (HasAuthority() && active && OtherActor->IsA(APlayableCharacter::StaticClass()))
	{
		EventManager::OnCheckpointReached.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("ResetDeathRecording"));
		active = false;
	}
}


void ACheckpoint::RechargePlayers()
{
	if (!HasAuthority()) {
		RPC_RechargePlayers();
	}
	else {
		Private_RechargePlayers();
	}
}

void ACheckpoint::RPC_RechargePlayers_Implementation()
{
	RechargePlayers();
}

bool ACheckpoint::RPC_RechargePlayers_Validate() {
	return true;
}

void ACheckpoint::Private_RechargePlayers() 
{
	if (m_triggered) {
		return;
	}

	m_triggered = true;

	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayableCharacter::StaticClass(), foundActors);

	for (AActor* actor : foundActors) {
		auto lifeComponent = actor->GetComponentByClass(UDamageableCharacterComponent::StaticClass());

		if (lifeComponent) {
			auto damageableComponent = Cast<UDamageableCharacterComponent>(lifeComponent);

			damageableComponent->SetHealth(damageableComponent->MaxHealth);
		}

		auto manaComponent = actor->GetComponentByClass(UEnergyManagerComponent::StaticClass());

		if (manaComponent) {
			auto energyComponent = Cast<UEnergyManagerComponent>(manaComponent);

			energyComponent->GiveEnergy(energyComponent->GetMaxEnergy());
		}

		if(actor->GetName().Contains("Warrior")) {
			auto warrior = Cast<AWarriorCharacter>(actor);

			warrior->ActivateHealingParticles();
		} else if(actor->GetName().Contains("Mage")) {
			auto mage = Cast<AMageCharacter>(actor);

			mage->ActivateHealingParticles();
		}
	}
}