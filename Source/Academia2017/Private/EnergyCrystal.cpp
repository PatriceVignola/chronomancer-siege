#include "Academia2017.h"
#include "EnergyManagerComponent.h"
#include "EventManager.h"
#include "Engine.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "EnergyCrystal.h"

AEnergyCrystal::AEnergyCrystal()
{
	PrimaryActorTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	RootComponent = Collider;

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	bReplicates = true;
}

void AEnergyCrystal::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Collider->OnComponentBeginOverlap.AddDynamic(this, &AEnergyCrystal::OnBeginOverlap);

		if (Rewindable)
		{
			EventManager::OnRecordPressed.AddUObject(this, &AEnergyCrystal::NoticeRecordStarted);
			EventManager::OnRecordReleased.AddUObject(this, &AEnergyCrystal::NoticeRecordStopped);
			EventManager::OnGameOver.AddUObject(this, &AEnergyCrystal::OnGameOver);
			EventManager::OnCheckpointReached.AddUObject(this, &AEnergyCrystal::SetDontDestroyAfterDeathRewind);
			EventManager::OnDeathRewindFinished.AddUObject(this, &AEnergyCrystal::CheckIfShouldDestroy);

			rewindManager = GetWorld()->GetGameState()->FindComponentByClass<URewindManagerComponent>();
		}
	}

	auto rotator = FRotator(FMath::RandRange(0.f, 360.f), FMath::RandRange(0.f, 360.f), FMath::RandRange(0.f, 360.f));

	if(HasAuthority()) {
		RPC_SetDefaultAngle(rotator);
	} else { 
		SetActorRotation(rotator);
	}

	FRotator newAngularVelocity = FRotator(FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f), FMath::RandRange(0.f, 1.f)).GetNormalized() * RotationSpeed;

	if (HasAuthority())
	{
		RPC_SetAngularVelocity(newAngularVelocity);
	} else {
		angularVelocity = newAngularVelocity;
	}
}

void AEnergyCrystal::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (!rewinding)
	{
		timeSinceSpawned += DeltaTime;
		if(HasAuthority()) {
			SetActorRotation(GetActorQuat() * (angularVelocity * DeltaTime).Quaternion());
		} else {
			RPC_Rotate(DeltaTime);
		}
	}

	if (HasAuthority() && Duration != 0.f && !dead && timeSinceSpawned >= Duration)
	{
		SetDeadState();
	}

	if (recording)
	{
		recordElapsedTime += DeltaTime;
	}
	else if (rewinding)
	{
		float rewindDelta = rewindManager ? rewindManager->GetRewindDelta() : DeltaTime;
		if(HasAuthority()) {
			SetActorRotation(GetActorQuat() * (angularVelocity * rewindDelta).Quaternion().Inverse());
		} else {
			RPC_RotateReverse(rewindDelta);
		}
		

		if (timeSinceSpawned <= 0)
		{
			if (HasAuthority())
			{
				DestroyEnergyCrystal();
				return;
			}
		}
		else if (recordElapsedTime <= 0)
		{
			rewinding = false;
		}

		if (HasAuthority() && dead && recordElapsedTime <= deadTimeStamp)
		{
			SetAliveState();
		}

		recordElapsedTime -= rewindDelta;
		timeSinceSpawned -= rewindDelta;
	}

	if (RespawnsInfinitely && timeSinceRespawned > 0.f)
	{
		timeSinceRespawned -= DeltaTime;

		if (timeSinceRespawned <= 0.f)
		{
			SetActorHiddenInGame(false);
			Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
}

void AEnergyCrystal::OnBeginOverlap(UPrimitiveComponent *ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (HasAuthority())
	{
		if (UEnergyManagerComponent *energyManager = OtherActor->FindComponentByClass<UEnergyManagerComponent>())
		{
			if (energyManager->GetRemainingEnergy() < energyManager->GetMaxEnergy())
			{
				energyManager->GiveEnergy(EnergyValue);
				SetDeadState();
			}
		}
	}
}

void AEnergyCrystal::NoticeRecordStarted(bool isFixedDuration, float duration)
{
	if (HasAuthority())
	{
		RPC_NoticeRecordStarted();
	}
}

void AEnergyCrystal::RPC_NoticeRecordStarted_Implementation()
{
	recording = true;
	recordElapsedTime = 0.f;
}

void AEnergyCrystal::NoticeRecordStopped()
{
	if (HasAuthority())
	{
		RPC_NoticeRecordStopped();
	}
}

void AEnergyCrystal::RPC_NoticeRecordStopped_Implementation()
{
	if (!recording)
	{
		recordElapsedTime = timeSinceSpawned;
	}

	recording = false;
	rewinding = true;
}

void AEnergyCrystal::SetDeadState()
{
	if (HasAuthority())
	{
		alreadyDroppedOnce = true;

		RPC_SpawnParticlesAndSound();

		if (RespawnsInfinitely)
		{
			SetActorHiddenInGame(true);
			Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			timeSinceRespawned = RespawnCooldown;
		}
		else if ((DropsOnlyOnce && alreadyDroppedOnce) || !Rewindable || !recording)
		{
			DestroyEnergyCrystal();
		}
		else
		{
			dead = true;
			deadTimeStamp = recordElapsedTime;
			Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetActorHiddenInGame(true);
		}
	}
}

void AEnergyCrystal::SetAliveState()
{
	if (HasAuthority())
	{
		dead = false;
		Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SetActorHiddenInGame(false);

		if (IsOverlappingEnergyManager())
		{
			SetDeadState();
		}
	}
}

void AEnergyCrystal::OnGameOver() {
	if(Duration >= 0.001f) {
		DestroyEnergyCrystal();
	}
}

void AEnergyCrystal::DestroyEnergyCrystal()
{
	if (HasAuthority())
	{
		Destroy();
	}
}

void AEnergyCrystal::RPC_SpawnParticlesAndSound_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticlesTemplate, GetActorLocation());
	PlayDestroySound();
}

void AEnergyCrystal::RPC_SetAngularVelocity_Implementation(const FRotator &newAngularVelocity)
{
	angularVelocity = newAngularVelocity;
}

bool AEnergyCrystal::IsOverlappingEnergyManager()
{
	TArray<FHitResult> outHits;

	FCollisionShape cubeShape;
	cubeShape.SetBox(Collider->GetScaledBoxExtent());

	GetWorld()->SweepMultiByObjectType(outHits,
		Collider->GetComponentLocation(),
		Collider->GetComponentLocation(),
		Collider->GetComponentQuat(),
		FCollisionObjectQueryParams(ECC_Pawn),
		cubeShape);

	for (FHitResult &hitResult : outHits)
	{
		if (hitResult.GetActor()->FindComponentByClass<UEnergyManagerComponent>())
		{
			return true;
		}
	}

	return false;
}

void AEnergyCrystal::SetDontDestroyAfterDeathRewind()
{
	if (HasAuthority())
	{
		dontDestroyAfterDeathRewind = true;
	}
}

void AEnergyCrystal::CheckIfShouldDestroy()
{
	if (HasAuthority() && !dontDestroyAfterDeathRewind)
	{
		Destroy();
	}
}

void AEnergyCrystal::RPC_Rotate_Implementation(float deltaTime) {
	SetActorRotation(GetActorQuat() * (angularVelocity * deltaTime).Quaternion());
}

void AEnergyCrystal::RPC_RotateReverse_Implementation(float rewindDelta) {
	SetActorRotation(GetActorQuat() * (angularVelocity * rewindDelta).Quaternion().Inverse());
}

void AEnergyCrystal::RPC_SetDefaultAngle_Implementation(const FRotator& rotator) {
	SetActorRotation(rotator);
}
