#include "Academia2017.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "TargetComponent.h"
#include "DrawDebugHelpers.h"
#include "MageCharacter.h"

UTargetComponent::UTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UActorComponent *comp = GetOwner()->FindComponentByClass(UEnergyManagerComponent::StaticClass()))
	{
		energyManager = Cast<UEnergyManagerComponent>(comp);
	}

	mesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();

	if (mesh)
	{
		animInstance = mesh->GetAnimInstance();
	}

	charMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();

	if (charMovement)
	{
		normalSpeed = charMovement->MaxWalkSpeed;
	}
}

void UTargetComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// Executed only once
	if (!inputComponent && GetOwner()->InputComponent)
	{
		inputComponent = GetOwner()->InputComponent;

		inputComponent->BindAction("CastSlowDome", IE_Pressed, this, &UTargetComponent::RequestStartCastingSlowDome);
		inputComponent->BindAction("CastSlowDome", IE_Released, this, &UTargetComponent::RequestStopCastingSlowDome);
	}

	if (GetOwner()->HasAuthority())
	{
		if (remainingSlowDomeCooldown > 0.f)
		{
			remainingSlowDomeCooldown -= DeltaTime;
		}

		if (currentSlowDome)
		{
			timeSinceCast += DeltaTime;

			FVector pos = currentSlowDome->GetRootComponent()->GetRelativeTransform().GetLocation();

			pos += FVector::RightVector * (SpeedCurve ? SpeedCurve->GetFloatValue(timeSinceCast) : defaultSpeed) * DeltaTime;

			float scaleRatio = FMath::Clamp(pos.Size() / MaxRange, 0.f, 1.f);

			currentSlowDome->SetActorScale3D(FVector(1, 1, 1) * FMath::Lerp(InitialScale, FinalScale, scaleRatio));
			currentSlowDome->SetActorRelativeLocation(pos);

			if (pos.Size() >= MaxRange)
			{
				StopCastingSlowDome();
			}
		}
	}
}

void UTargetComponent::RequestStartCastingSlowDome()
{
	if (!GetOwner()->HasAuthority())
	{
		Server_RequestStartCastingSlowDome();
	}
	else
	{
		StartCastingSlowDome();
	}
}

void UTargetComponent::Server_RequestStartCastingSlowDome_Implementation()
{
	if (GetOwner()->HasAuthority())
	{
		StartCastingSlowDome();
	}
}

bool UTargetComponent::Server_RequestStartCastingSlowDome_Validate()
{
	return true;
}

void UTargetComponent::StartCastingSlowDome()
{
	if (GetOwner()->HasAuthority() && SlowDomeClass && remainingSlowDomeCooldown <= 0.f)
	{
		currentSlowDome = GetWorld()->SpawnActor<ASlowDome>(SlowDomeClass, GetOwner()->GetActorLocation(), FRotator::ZeroRotator);

		if (energyManager && currentSlowDome->GetEnergyCost() > energyManager->GetRemainingEnergy())
		{
			currentSlowDome->Destroy();
			currentSlowDome = nullptr;
		}
		else
		{
			energyManager->ConsumeEnergy(currentSlowDome->GetEnergyCost());
			remainingSlowDomeCooldown = currentSlowDome->GetDomeCooldown();
			timeSinceCast = 0.f;
			FAttachmentTransformRules attachRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, false);
			currentSlowDome->GetRootComponent()->AttachToComponent(mesh, attachRules);
			
			RPC_PlayCastAnimation();
		}
	}
}

void UTargetComponent::RequestStopCastingSlowDome()
{
	if (!GetOwner()->HasAuthority())
	{
		Server_RequestStopCastingSlowDome();
	}
	else
	{
		StopCastingSlowDome();
	}
}

void UTargetComponent::Server_RequestStopCastingSlowDome_Implementation()
{
	if (GetOwner()->HasAuthority())
	{
		StopCastingSlowDome();
	}
}

bool UTargetComponent::Server_RequestStopCastingSlowDome_Validate()
{
	return true;
}

void UTargetComponent::StopCastingSlowDome()
{
	if (GetOwner()->HasAuthority() && currentSlowDome)
	{
		FDetachmentTransformRules detachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
		currentSlowDome->GetRootComponent()->DetachFromComponent(detachRules);
		currentSlowDome->StartLosingLife();

		currentSlowDome = nullptr;

		RPC_StopCastAnimation();
	}
}


void UTargetComponent::RPC_PlayCastAnimation_Implementation()
{
	if (charMovement)
	{
		charMovement->MaxWalkSpeed = normalSpeed * MageMovementRatioDuringCast;
	}

	if (animInstance)
	{
		animInstance->Montage_Play(MagicCastingMontage);
	}

	PlayCastSound();
}

void UTargetComponent::RPC_StopCastAnimation_Implementation()
{
	if (charMovement)
	{
		charMovement->MaxWalkSpeed = normalSpeed;
	}

	if (animInstance)
	{
		animInstance->Montage_Stop(0.5f, MagicCastingMontage);
	}
}