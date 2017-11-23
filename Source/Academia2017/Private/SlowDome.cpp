#include "Academia2017.h"
#include "WarriorCharacter.h"
#include "MageCharacter.h"
#include "SlowDome.h"

ASlowDome::ASlowDome()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetupAttachment(RootComponent);
	RootComponent = Root;

	SlowZone = CreateDefaultSubobject<USphereComponent>(TEXT("SlowZone"));
	SlowZone->SetupAttachment(RootComponent);

	SlowParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SlowParticles"));
	SlowParticles->SetupAttachment(RootComponent);
}

void ASlowDome::BeginPlay()
{
	Super::BeginPlay();

	SlowZone->OnComponentBeginOverlap.AddDynamic(this, &ASlowDome::StartSlowing);
	SlowZone->OnComponentEndOverlap.AddDynamic(this, &ASlowDome::EndSlowing);
}

void ASlowDome::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ASlowDome::StartSlowing(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor->IsA(AWarriorCharacter::StaticClass()) || OtherActor->IsA(AMageCharacter::StaticClass()))
	{
		return;
	}

	OtherActor->CustomTimeDilation = 1 - SlowPercent;
}

void ASlowDome::EndSlowing(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AWarriorCharacter::StaticClass()) || OtherActor->IsA(AMageCharacter::StaticClass()))
	{
		return;
	}

	OtherActor->CustomTimeDilation = 1;
}

float ASlowDome::GetDomeCooldown()
{
	return DomeCooldown;
}

float ASlowDome::GetEnergyCost()
{
	return EnergyCost;
}

void ASlowDome::StartLosingLife()
{
	if (HasAuthority())
	{
		SetLifeSpan(DomeDuration);
	}
}