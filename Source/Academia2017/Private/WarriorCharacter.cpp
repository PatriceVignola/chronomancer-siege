#include "Academia2017.h"
#include "MenuSpawnerComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "WarriorCharacter.h"

AWarriorCharacter::AWarriorCharacter()
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

	bReplicates = true;
}

void AWarriorCharacter::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();
	timeEventManager = FindComponentByClass<UTimeEventManagerComponent>();
	attackHitbox = FindComponentByClass<UAttackHitbox>();
	if (HasAuthority())
	{
		if (AnimInstance)
		{
			AnimInstance->RootMotionMode = ERootMotionMode::RootMotionFromEverything;
		}
	}	
	else if (AnimInstance)
	{
		AnimInstance->RootMotionMode = ERootMotionMode::IgnoreRootMotion;
	}

	if (AnimInstance)
	{
		AnimInstance->OnMontageBlendingOut.AddDynamic(this, &AWarriorCharacter::AttackMontageBleedingOut);
	}
}

void AWarriorCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWarriorCharacter, AnimVelocity);
}

void AWarriorCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AWarriorCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWarriorCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWarriorCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWarriorCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("LightAttack", IE_Pressed,this, &AWarriorCharacter::DoLightAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AWarriorCharacter::DoHeavyAttack);
}

void AWarriorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		AnimVelocity = GetCharacterMovement()->Velocity;

		if (blockMovementInput && (!AnimInstance || AnimInstance->IsAnyMontagePlaying()))
		{
			blockMovementInput = false;
		}
	}
}

void AWarriorCharacter::DoLightAttack()
{
	DoLightAttack(inputDirection);
}

void AWarriorCharacter::DoLightAttack(FVector direction)
{
	if (HasAuthority())
	{
		if (!m_IsOnCooldown && LightAttackAnims.Num() > 0)
		{
			if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
			{
				if (attackHitbox)
				{
					attackHitbox->BaseDamage = LightAttackDamage;
				}

				LightAttackIndex = 0;
				RPC_PlayLightAttackAnim(LightAttackIndex, direction);
			}
			else if (CanCombo)
			{
				LightAttackQueued = true;
				LightAttackQueuedDirection = direction;
			}
		}
	}
	else
	{
		Server_RequestDoLightAttack(inputDirection);

		if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
		{
			blockMovementInput = true;
		}
	}
}

void AWarriorCharacter::Server_RequestDoLightAttack_Implementation(FVector lastInputDir)
{
	if (HasAuthority())
	{
		DoLightAttack(lastInputDir);
	}
}

bool AWarriorCharacter::Server_RequestDoLightAttack_Validate(FVector lastInputDir)
{
	return true;
}

void AWarriorCharacter::DoHeavyAttack()
{
	if (HasAuthority())
	{
		if (!m_IsOnCooldown && HeavyAttackAnims.Num() > 0)
		{
			if (!AnimInstance->IsAnyMontagePlaying())
			{
				if (attackHitbox)
				{
					attackHitbox->BaseDamage = HeavyAttackDamage;
				}

				HeavyAttackIndex = 0;
				RPC_PlayHeavyAttackAnim(HeavyAttackIndex);
			}
			else if (CanCombo)
			{
				HeavyAttackQueued = true;
			}
		}
	}
	else
	{
		Server_RequestDoHeavyAttack();

		if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
		{
			blockMovementInput = true;
		}
	}
}

void AWarriorCharacter::ActivateHealingParticles() {
	if(HasAuthority()) {
		RPC_ActivateHealingParticles();
	}
}

void AWarriorCharacter::Server_RequestDoHeavyAttack_Implementation()
{
	if (HasAuthority())
	{
		DoHeavyAttack();
	}
}

bool AWarriorCharacter::Server_RequestDoHeavyAttack_Validate()
{
	return true;
}

void AWarriorCharacter::RPC_PlayHeavyAttackAnim_Implementation(int AttackIndex)
{
	blockMovementInput = false;

	if (attackHitbox)
	{
		attackHitbox->CanDamage = true;
	}

	if (AnimInstance && AttackIndex < HeavyAttackAnims.Num())
	{
		AnimInstance->Montage_Play(HeavyAttackAnims[AttackIndex]);
		PlayHeavyAttackSound(AttackIndex);
		timeEventManager->OnAttackStarted.Broadcast(LightAttackAnims.Num() + AttackIndex);
	}
}

void AWarriorCharacter::RPC_PlayLightAttackAnim_Implementation(int AttackIndex, FVector direction)
{
	blockMovementInput = false;

	if (attackHitbox)
	{
		attackHitbox->CanDamage = true;
	}
		
	if (AnimInstance && AttackIndex < LightAttackAnims.Num())
	{
		AnimInstance->Montage_Play(LightAttackAnims[AttackIndex]);
		PlayLightAttackSound(AttackIndex);
		timeEventManager->OnAttackStarted.Broadcast(AttackIndex);
	}

	if (HasAuthority() && LightAttackQueuedDirection != FVector::ZeroVector)
	{
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + direction));
	}
}

void AWarriorCharacter::AttackMontageBleedingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (attackHitbox)
		attackHitbox->CanDamage = false;
	if (HasAuthority())
	{
		if (LightAttackQueued)
		{
			if (attackHitbox)
			{
				attackHitbox->BaseDamage = LightAttackDamage;
			}

			LightAttackIndex += 1;
			LightAttackIndex %= LightAttackAnims.Num();
			RPC_PlayLightAttackAnim(LightAttackIndex, LightAttackQueuedDirection);
			LightAttackQueued = false;
		}
		else if (HeavyAttackQueued)
		{
			if (attackHitbox)
			{
				attackHitbox->BaseDamage = HeavyAttackDamage;
			}

			HeavyAttackIndex += 1;
			HeavyAttackIndex %= HeavyAttackAnims.Num();
			RPC_PlayHeavyAttackAnim(HeavyAttackIndex);
			HeavyAttackQueued = false;
		}

		CanCombo = false;

		if (LightAttackIndex == LightAttackAnims.Num())
		{
			LightAttackIndex = 0;
			m_Timer = AttackCooldown;
			m_IsOnCooldown = true;
		}

		if (HeavyAttackIndex == HeavyAttackAnims.Num())
		{
			HeavyAttackIndex = 0;
			m_Timer = AttackCooldown;
			m_IsOnCooldown = true;
		}
	}
}

void AWarriorCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWarriorCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AWarriorCharacter::MoveForward(float Value)
{
	if (frameFirstInput)
	{
		inputDirection = FVector::ZeroVector;
	}

	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		if (HasAuthority() || !AnimInstance || (!AnimInstance->IsAnyMontagePlaying() && !blockMovementInput))
		{
			AddMovementInput(Direction, Value);
		}

		inputDirection += Direction * FMath::Sign(Value);
		ForwardDirection = Direction * Value;
	}
	else
	{
		ForwardDirection = FVector::ZeroVector;
	}

	frameFirstInput = !frameFirstInput;
}

void AWarriorCharacter::MoveRight(float Value)
{
	if (frameFirstInput)
	{
		inputDirection = FVector::ZeroVector;
	}

	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction

		if (HasAuthority() || (!AnimInstance->IsAnyMontagePlaying() && !blockMovementInput))
		{
			AddMovementInput(Direction, Value);
		}

		inputDirection += Direction * FMath::Sign(Value);
		RightDirection = Direction * Value;
	}
	else
	{
		RightDirection = FVector::ZeroVector;
	}

	frameFirstInput = !frameFirstInput;
}

void AWarriorCharacter::RPC_ActivateHealingParticles_Implementation()
{
	BPActivateHealingParticles();
}