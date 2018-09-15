#include "Academia2017.h"
#include "BossEgg.h"
#include "DestructibleComponent.h"

ABossEgg::ABossEgg()
{
	PrimaryActorTick.bCanEverTick = true;

	DestructableMesh = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructableMesh"));
	DestructableMesh->SetupAttachment(RootComponent);
	BodyHitBox = CreateDefaultSubobject<UActivableHitbox>(TEXT("BodyHitBox"));
	BodyHitBox->SetupAttachment(DestructableMesh);

	bReplicates = true;
}

void ABossEgg::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		m_Timer = DelayBeforeSpawn;
		DestructableMesh->SetEnableGravity(true);
	}
}

void ABossEgg::BreakEgg()
{
	// Extra ultra safe check to avoid crashes during the presentation
	if (!HasAuthority() || IsPendingKill() || !IsValidLowLevel()) return;

	shouldDestroy = true;
	destroyTimer = 1.f;
	RPC_PlayCrackSound();
}

void ABossEgg::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Extra ultra safe check to avoid crashes during the presentation
	if (!HasAuthority() || IsPendingKill() || !IsValidLowLevel()) return;

	m_Timer -= DeltaTime;

	if (!broken && BodyHitBox->IsActivated)
	{
		DestructableMesh->ApplyDamage(1000, BodyHitBox->HitLocation, this->GetActorLocation() - BodyHitBox->HitLocation, 100);
		BreakEgg();
		m_IsNeutralized = true;
		broken = true;
	}
	else if (!m_IsNeutralized && !broken && m_Timer <= 0.0f)
	{
		DestructableMesh->ApplyDamage(1000, this->GetActorLocation(), this->GetActorLocation(), 100);
		broken = true;
		BreakEgg();

		if (HasAuthority())
		{
			SpawnEnemies();
		}
	}

	if (shouldDestroy)
	{
		destroyTimer -= DeltaTime;

		if (destroyTimer <= 0.f)
		{
			Destroy();
		}
	}
}

void ABossEgg::SpawnEnemies()
{
	// Extra ultra safe check to avoid crashes during the presentation
	if (!HasAuthority() || IsPendingKill() || !IsValidLowLevel()) return;

	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (EnemySpawnedClass)
	{
		FVector location = GetActorLocation();

		for (int i = 0; i < EnemyCount; ++i)
		{
			GetWorld()->SpawnActor<AEnemyBase>(EnemySpawnedClass, location, FRotator::ZeroRotator, param);
		}
	}

	shouldDestroy = true;
	destroyTimer = 0.5f;
}

void ABossEgg::RPC_PlayCrackSound_Implementation()
{
	PlayCrackSound();
}