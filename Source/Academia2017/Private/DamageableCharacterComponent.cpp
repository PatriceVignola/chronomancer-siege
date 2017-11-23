#include "Academia2017.h"
#include "DamageableCharacterComponent.h"
#include "PlayableCharacter.h"
#include "MageCharacter.h"
#include "WarriorCharacter.h"
#include "EventManager.h"

UDamageableCharacterComponent::UDamageableCharacterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	MaxHealth = 100;
	Health = MaxHealth;
}
void UDamageableCharacterComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDamageableCharacterComponent, Health);
	DOREPLIFETIME(UDamageableCharacterComponent, IsDead);
}

void UDamageableCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		if (GetOwner()->IsA(AMageCharacter::StaticClass()) || GetOwner()->IsA(AWarriorCharacter::StaticClass()))
		{
			EventManager::OnWarriorChangedHealth.AddUObject(this, &UDamageableCharacterComponent::NotifyWarriorHealthChanged);
			EventManager::OnMageChangedHealth.AddUObject(this, &UDamageableCharacterComponent::NotifyMageHealthChanged);
		}

		timeEventManager = GetOwner()->FindComponentByClass<UTimeEventManagerComponent>();

		EventManager::OnGameOver.AddUObject(this, &UDamageableCharacterComponent::SetInvulnerableState);
		EventManager::OnRecordReleased.AddUObject(this, &UDamageableCharacterComponent::SetInvulnerableState);
		EventManager::OnDeathRewindFinished.AddUObject(this, &UDamageableCharacterComponent::SetVulnerableState);
		EventManager::OnRewindFinished.AddUObject(this, &UDamageableCharacterComponent::SetVulnerableState);
		EventManager::OnDeathRewindFinished.AddUObject(this, &UDamageableCharacterComponent::RefillLife);
	}
}

void UDamageableCharacterComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!inputComponent && GetOwner()->InputComponent)
	{
		inputComponent = GetOwner()->InputComponent;

		inputComponent->BindAction("Suicide_DEBUG", IE_Pressed, this, &UDamageableCharacterComponent::Kill);
		inputComponent->BindAction("Invulnerable_DEBUG", IE_Pressed, this, &UDamageableCharacterComponent::SetInvulnerableHack);
	}
}

void UDamageableCharacterComponent::Damage(float damagePoints)
{
	if (GetOwner()->HasAuthority() && !invulnerable)
	{
		SetHealth(Health - damagePoints);
	}
}

void UDamageableCharacterComponent::SetHealth(float newHealth)
{
	if (!GetOwner()->HasAuthority()) return;

	float prevHealth = Health;
	Health = FMath::Clamp(newHealth, 0.f, MaxHealth);

	if (prevHealth != Health)
	{
		if (prevHealth > Health)
		{
			RPC_NotifyDamageTaken();
		}

		if (IsDead && newHealth > 0)
		{
			SetAliveState();
		}
		else if (!IsDead && newHealth <= 0)
		{
			SetDeadState();
		}

		NotifyHealthChanged();
	}
}

void UDamageableCharacterComponent::Kill()
{
	if (GetOwner()->HasAuthority() && Health > 0)
	{
		Damage(Health);
	}
	else
	{
		Server_RequestKill();
	}
}

void UDamageableCharacterComponent::Server_RequestKill_Implementation()
{
	Kill();
}

bool UDamageableCharacterComponent::Server_RequestKill_Validate()
{
	return true;
}

void UDamageableCharacterComponent::SetDeadState()
{
	if (GetOwner()->IsA(APlayableCharacter::StaticClass()) && !CanGameOver) return;

	if (GetOwner()->HasAuthority() && !IsDead)
	{
		IsDead = true;

		if (DropsEnergy && EnergyCrystalClass)
		{
			if (FMath::RandRange(0.f, 1.f) <= EnergyDropChance)
			{
				GetWorld()->SpawnActor<AEnergyCrystal>(EnergyCrystalClass, GetOwner()->GetActorLocation(), FRotator::ZeroRotator);
			}
		}

		if (timeEventManager)
		{
			timeEventManager->OnCharacterDied.Broadcast();
		}

		if (GetOwner()->IsA(APlayableCharacter::StaticClass()))
		{
			EventManager::OnCharacterDied.Broadcast();
		}

		RPC_NotifyDied();
	}
}

void UDamageableCharacterComponent::SetAliveState()
{
	if (GetOwner()->HasAuthority())
	{
		IsDead = false;
		RPC_NotifyRevived();
	}
}

void UDamageableCharacterComponent::NotifyHealthChanged()
{
	OnHealthChanged.Broadcast(Health);

	if (GetOwner()->IsA(AWarriorCharacter::StaticClass()))
	{
		EventManager::OnWarriorChangedHealth.Broadcast(Health, MaxHealth);
	}
	else if (GetOwner()->IsA(AMageCharacter::StaticClass()))
	{
		EventManager::OnMageChangedHealth.Broadcast(Health, MaxHealth);
	}
}

void UDamageableCharacterComponent::RPC_NotifyDied_Implementation()
{
	if (APlayableCharacter *playableCharacter = Cast<APlayableCharacter>(GetOwner()))
	{
		playableCharacter->PlayDeathAnimation();

		if (AController *controller = GetOwner()->GetInstigatorController())
		{
			if (APlayerController *playerController = Cast<APlayerController>(controller))
			{
				GetOwner()->DisableInput(playerController);
			}
		}
	}

	OnDied.Broadcast();
}

void UDamageableCharacterComponent::RPC_NotifyRevived_Implementation()
{
	if (APlayableCharacter *playableCharacter = Cast<APlayableCharacter>(GetOwner()))
	{
		if (AController *controller = GetOwner()->GetInstigatorController())
		{
			if (APlayerController *playerController = Cast<APlayerController>(controller))
			{
				GetOwner()->EnableInput(playerController);
			}
		}
	}

	OnRevived.Broadcast();
}

void UDamageableCharacterComponent::SetInvulnerableState()
{
	if (GetOwner()->HasAuthority())
	{
		invulnerable = true;
	}
}

void UDamageableCharacterComponent::SetVulnerableState()
{
	if (GetOwner()->HasAuthority())
	{
		invulnerable = false;
	}
}

void UDamageableCharacterComponent::NotifyWarriorHealthChanged(float currentHealth, float OtherPlayerMaxHealth)
{
	if (GetOwner()->HasAuthority())
	{
		RPC_NotifyWarriorHealthChanged(currentHealth, OtherPlayerMaxHealth);
	}
	else
	{
		Server_NotifyWarriorHealthChanged(currentHealth, OtherPlayerMaxHealth);
	}
}

void UDamageableCharacterComponent::RPC_NotifyWarriorHealthChanged_Implementation(float currentHealth, float OtherPlayerMaxHealth)
{
	OnWarriorHealthChange.Broadcast(currentHealth, OtherPlayerMaxHealth);
}

void UDamageableCharacterComponent::NotifyMageHealthChanged(float currentHealth, float OtherPlayerMaxHealth)
{
	if (GetOwner()->HasAuthority())
	{
		RPC_NotifyMageHealthChanged(currentHealth, OtherPlayerMaxHealth);
	}
	else
	{
		Server_NotifyMageHealthChanged(currentHealth, OtherPlayerMaxHealth);
	}
}

void UDamageableCharacterComponent::RPC_NotifyMageHealthChanged_Implementation(float currentHealth, float OtherPlayerMaxHealth)
{
	OnMageHealthChange.Broadcast(currentHealth, OtherPlayerMaxHealth);
}

void UDamageableCharacterComponent::Server_NotifyMageHealthChanged_Implementation(float currentHealth, float OtherPlayerMaxHealth)
{
	NotifyMageHealthChanged(currentHealth, OtherPlayerMaxHealth);
}

bool UDamageableCharacterComponent::Server_NotifyMageHealthChanged_Validate(float currentHealth, float OtherPlayerMaxHealth)
{
	return true;
}

void UDamageableCharacterComponent::Server_NotifyWarriorHealthChanged_Implementation(float currentHealth, float OtherPlayerMaxHealth)
{
	NotifyWarriorHealthChanged(currentHealth, OtherPlayerMaxHealth);
}

bool UDamageableCharacterComponent::Server_NotifyWarriorHealthChanged_Validate(float currentHealth, float OtherPlayerMaxHealth)
{
	return true;
}

void UDamageableCharacterComponent::RPC_NotifyDamageTaken_Implementation()
{
	OnDamageTaken.Broadcast();
}

void UDamageableCharacterComponent::SetInvulnerableHack()
{
	if (!GetOwner()->HasAuthority())
	{
		Server_SetInvulnerableHack();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is now invulnerable!"));
		CanGameOver = false;
	}
}

void UDamageableCharacterComponent::Server_SetInvulnerableHack_Implementation()
{
	SetInvulnerableHack();
}

bool UDamageableCharacterComponent::Server_SetInvulnerableHack_Validate()
{
	return true;
}

void UDamageableCharacterComponent::RefillLife()
{
	SetHealth(MaxHealth);
}