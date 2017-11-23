#include "Academia2017.h"
#include "AttackHitbox.h"
#include "DamagableHitbox.h"
#include "PlayableCharacter.h"
#include "AttackHitbox.h"
#include "PlayerBucketComponent.h"
#include "AIBucketComponent.h"
#include "DrawDebugHelpers.h"
#include "EnemyBase.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
    MeleeRangeBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeRangeBounds"));
	MeleeRangeBounds->SetupAttachment(RootComponent);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageBlendingOut.AddDynamic(this, &AEnemyBase::AttackMontageBleedingOut);
		AnimInstance->OnMontageEnded.AddDynamic(this, &AEnemyBase::AttackMontageBleedingOut);
	}
}

void AEnemyBase::AttackMontageBleedingOut(UAnimMontage* Montage, bool bInterrupted)
{
	TArray<UActorComponent*> attackHitboxes = GetComponentsByClass(UAttackHitbox::StaticClass());
	for (int i = 0; i < attackHitboxes.Num(); i++) {
		UActorComponent* comp = attackHitboxes[i];
		if (UAttackHitbox* attackHitbox = Cast<UAttackHitbox>(comp))
		{
			attackHitbox->CanDamage = false;
		}

	}
}

void AEnemyBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

AActor* AEnemyBase::GetTarget(TArray<AActor*> VisibleCharacters)
{
	UAIBucketComponent* AIBucketComp = nullptr;
	if (UActorComponent *comp = this->FindComponentByClass(UAIBucketComponent::StaticClass())) {
		AIBucketComp = Cast<UAIBucketComponent>(comp);
	}

	for (int i = 0; i < VisibleCharacters.Num(); ++i)
	{
		for (int j = 0; j < VisibleCharacters.Num(); ++j)
		{
			FVector current= GetActorLocation() - VisibleCharacters[i]->GetActorLocation();
			FVector next = GetActorLocation() - VisibleCharacters[j]->GetActorLocation();
			if (current.Size() < next.Size())
			{
				AActor* temp = VisibleCharacters[i];
				VisibleCharacters[i] = VisibleCharacters[j];
				VisibleCharacters[j] = temp;
			}
		}
		
	}

	if (!AIBucketComp &&VisibleCharacters.Num()>0)
	{
		return VisibleCharacters[0];
	}

	for (int i = 0; i < VisibleCharacters.Num(); ++i)
	{
		FVector DistanceVector = GetActorLocation() - VisibleCharacters[i]->GetActorLocation();
			if (UActorComponent *comp = VisibleCharacters[i]->FindComponentByClass(UPlayerBucketComponent::StaticClass())) {
				UPlayerBucketComponent* playerBucket = Cast<UPlayerBucketComponent>(comp);
				if (playerBucket != nullptr)
				{
					if (playerBucket->IsFull(AIBucketComp->BucketValue))
					{
						continue;
					}
					else
					{
						AIBucketComp->RegisterBucket(playerBucket);
						return VisibleCharacters[i];
					}
				}

			}
	}

	return VisibleCharacters.Num() > 0 ? VisibleCharacters[0] : nullptr;
}

FVector AEnemyBase::CalculateTacticalRetreat(TArray<AActor*> EnemyCharacters, TArray<AActor*> FriendlyCharacters,float DistanceBehindTroop,float FloorZ,float MaxSize)
{
	FVector EnemyCharactersCenterOfMass = FVector(0,0,0);
	FVector FriendlyCharactersCenterOfMass = FVector(0, 0, 0);
	FVector DirectionVector = FVector(0, 0, 0);;
	float OriginalLength;

	for (int i = 0; i < EnemyCharacters.Num(); ++i)
	{
		EnemyCharactersCenterOfMass += EnemyCharacters[i]->GetActorLocation();
	}

	EnemyCharactersCenterOfMass /= EnemyCharacters.Num();
	
	for (int i = 0; i < FriendlyCharacters.Num(); ++i)
	{
		FriendlyCharactersCenterOfMass = FriendlyCharacters[i]->GetActorLocation();
	}

	FriendlyCharactersCenterOfMass /= FriendlyCharacters.Num();
	EnemyCharactersCenterOfMass.Z = FloorZ;
	FriendlyCharactersCenterOfMass.Z  = FloorZ;
	DirectionVector =   FriendlyCharactersCenterOfMass - EnemyCharactersCenterOfMass;

	OriginalLength = DirectionVector.Size();
	DirectionVector = DirectionVector.GetSafeNormal();
	float VectorSize =OriginalLength + DistanceBehindTroop; // FMath::Clamp((OriginalLength + DistanceBehindTroop),0.0f,MaxSize);
	FVector Destination = EnemyCharactersCenterOfMass + DirectionVector * (VectorSize);
	return Destination;
}

void AEnemyBase::Attack(bool isUnderBelly )
{
	if (!HasAuthority())
	{
		Server_Attack(isUnderBelly);
	}
	else
	{
		RPC_Attack(isUnderBelly);
	}
}

void AEnemyBase::Server_Attack_Implementation(bool isUnderBelly = false)
{
	RPC_Attack(isUnderBelly);
}

bool AEnemyBase::Server_Attack_Validate(bool isUnderBelly = false)
{
	return true;
}

void AEnemyBase::RPC_Attack_Implementation(bool isUnderBelly = false)
{
	TArray<UActorComponent*> attackHitboxes = GetComponentsByClass(UAttackHitbox::StaticClass());
	/*for (int i = 0; i < attackHitboxes.Num(); i++) {
		UActorComponent* comp = attackHitboxes[i];
		if (UAttackHitbox* attackHitbox = Cast<UAttackHitbox>(comp))
		{
			attackHitbox->CanDamage = true;
		}
	}*/
	if (isUnderBelly)
	{
		if (UnderBellyAttack)
		{
			if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
			{
				if(AttackMontage)
					AnimInstance->Montage_Play(UnderBellyAttack);
			}
		}
	}
	else if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
	{
		if(AttackMontage)
			AnimInstance->Montage_Play(AttackMontage);
	}
} 

bool AEnemyBase::RPC_Attack_Validate(bool isUnderBelly = false)
{
	return true;
}