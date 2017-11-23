// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Academia2017.h"
#include "EventManager.h"
#include "EnergyManagerComponent.h"
#include "Engine.h"
#include "MenuSpawnerComponent.h"
#include "MageCharacter.h"

//////////////////////////////////////////////////////////////////////////
// AMageCharacter

AMageCharacter::AMageCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;


	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

												// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//AnimComponent = CreateDefaultSubobject<UAnimComponent>(TEXT("AnimComponent"));
	//AddOwnedComponent(AnimComponent);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AMageCharacter::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = this->GetMesh()->GetAnimInstance();
	NotifyMageSpawned();
}

void AMageCharacter::NotifyMageSpawned()
{
	if (HasAuthority())
	{
		EventManager::OnMageSpawned.Broadcast();
	}
	else
	{
		Server_NotifyMageSpawned();
	}
}

void AMageCharacter::Server_NotifyMageSpawned_Implementation()
{
	if (HasAuthority())
	{
		NotifyMageSpawned();
	}
}

bool AMageCharacter::Server_NotifyMageSpawned_Validate()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMageCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	/*PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);*/

	PlayerInputComponent->BindAxis("MoveForward", this, &AMageCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMageCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMageCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMageCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("CastHealingDome", IE_Pressed, this, &AMageCharacter::RequestCastHealingDome);
}

void AMageCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 
}

void AMageCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMageCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if(CameraBoom) {
		auto rot = CameraBoom->GetComponentRotation();
		if(Rate > 0 || (Rate < 0 && rot.Pitch <= 1.f)) {
			AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());\
		}
	}
}

void AMageCharacter::RequestCastHealingDome()
{
	if(!HasAuthority()) {
		Server_RequestCastHealingDome();
	} else {
		CastHealingDome();
	}
}

void AMageCharacter::Server_RequestCastHealingDome_Implementation() 
{
	if (HasAuthority()) {
		CastHealingDome();
	}
}

bool AMageCharacter::Server_RequestCastHealingDome_Validate() 
{
	return true;
}

void AMageCharacter::CastHealingDome() 
{
	if(HasAuthority()) {
		if(!HealingDomeClass) {
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("HEALING DOME CLASS NOT SET"));
			return;
		}
		auto healingDome = GetWorld()->SpawnActor<AHealingDome>(HealingDomeClass, GetActorLocation(), FRotator::ZeroRotator);
		if(!healingDome) {
			return;
		}

		auto energyManagerComp = FindComponentByClass(UEnergyManagerComponent::StaticClass());
		
		if(!energyManagerComp) {
			return;
		}

		auto energyManager = Cast<UEnergyManagerComponent>(energyManagerComp);

		if(healingDome->GetEnergyCost() > energyManager->GetRemainingEnergy()) {
			healingDome->SetShouldntHeal();
			healingDome->Destroy();
			healingDome = nullptr;
		} else {
			RPC_PlayHealingSound();

			energyManager->ConsumeEnergy(healingDome->GetEnergyCost());
			healingDome->StartRemoval();
		}
	}
}

void AMageCharacter::ActivateHealingParticles() {
	if(HasAuthority()) {
		RPC_ActivateHealingParticles();

	}
}

void AMageCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMageCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMageCharacter::RPC_StartRunningOnServer_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed *= RunMultiplier;
}

bool AMageCharacter::RPC_StartRunningOnServer_Validate()
{
	return true;
}

void AMageCharacter::RPC_StopRunningOnServer_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed /= RunMultiplier;
}

bool AMageCharacter::RPC_StopRunningOnServer_Validate()
{
	return true;
}

void AMageCharacter::RPC_PlayHealingSound_Implementation()
{
	PlayHealingSound();
}

void AMageCharacter::RPC_ActivateHealingParticles_Implementation() 	{
	BPActivateHealingParticles();
}