// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "Engine.h"
#include "EnemyBase.h"
#include "WarriorCharacter.h"
#include "MageCharacter.h"
#include "DamagableHitbox.h"
#include "DamageableCharacterComponent.h"
#include "HealingDome.h"


// Sets default values
AHealingDome::AHealingDome()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetupAttachment(RootComponent);
	RootComponent = Root;

	HealingZone = CreateDefaultSubobject<USphereComponent>(TEXT("HealingZone"));
	HealingZone->SetupAttachment(RootComponent);

	HealingParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HealingParticles"));
	HealingParticles->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHealingDome::BeginPlay()
{
	Super::BeginPlay();
	HealingZone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (HasAuthority())
	{
		HealingZone->OnComponentBeginOverlap.AddDynamic(this, &AHealingDome::HealPlayer);
		HealingZone->OnComponentBeginOverlap.AddDynamic(this, &AHealingDome::StartDamageEnemy);
		HealingZone->OnComponentEndOverlap.AddDynamic(this, &AHealingDome::StopDamageEnemy);
	}
}

// Called every frame
void AHealingDome::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if(!m_passFirstTick)
	{
		HealingZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		m_passFirstTick = true;
	}
	
	if (!HasAuthority()) return;

	for(auto& actorToDamage : m_actorsToDamage) {
		if(actorToDamage.isStaggered) {
			actorToDamage.timeCount += DeltaTime;

			if(actorToDamage.timeCount >= DelayBetweenStagger) {
				actorToDamage.timeCount = 0.0f;
				actorToDamage.isStaggered = false;
			}
		} else {
			actorToDamage.isStaggered = true;

			if(!actorToDamage.actor || (actorToDamage.actor && actorToDamage.actor->IsActorBeingDestroyed())) {
				continue;
			}

			auto hitboxComp = actorToDamage.actor->GetComponentByClass(UDamagableHitbox::StaticClass());
			if(hitboxComp) {
				auto damageableHitBox = Cast<UDamagableHitbox>(hitboxComp);
				damageableHitBox->HasBeenHit = true;

				auto damageComp = actorToDamage.actor->GetComponentByClass(UDamageableCharacterComponent::StaticClass());
				if(damageComp) {
					auto damageableComponent = Cast<UDamageableCharacterComponent>(damageComp);
					damageableComponent->SetHealth(damageableComponent->Health - DamageAmount);
				}
			}

		}
	}
}


float AHealingDome::GetEnergyCost() {
	return EnergyCost;
}

void AHealingDome::StartRemoval() {
	if(HasAuthority()) {
		SetLifeSpan(SpellDuration);
		if(m_warriorCharacter) {
			m_warriorCharacter->ActivateHealingParticles();
		}
		if(m_mageCharacter) {
			m_mageCharacter->ActivateHealingParticles();
		}
	}
}

void AHealingDome::HealPlayer(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) {
	if(!HasAuthority() || OtherActor->IsA(AEnemyBase::StaticClass()) || shouldntHeal) {
		return;
	}

	auto comp = OtherActor->GetComponentByClass(UDamageableCharacterComponent::StaticClass());

	if(comp) {
		auto damageableComponent = Cast<UDamageableCharacterComponent>(comp);

		damageableComponent->SetHealth(damageableComponent->Health + HealingAmount);
		/*damageableComponent->Health += HealingAmount;
		if(damageableComponent->Health > damageableComponent->MaxHealth) {
			damageableComponent->Health = damageableComponent->MaxHealth;
		}*/

		if(OtherActor->GetName().Contains("Warrior")) {
			auto warrior = Cast<AWarriorCharacter>(OtherActor);
			m_warriorCharacter = warrior;
		} else if(OtherActor->GetName().Contains("Mage")) {
			auto mage = Cast<AMageCharacter>(OtherActor);
			m_mageCharacter = mage;
		}
	}
}

void AHealingDome::StartDamageEnemy(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) {
	if(OtherActor->IsA(AMageCharacter::StaticClass()) || OtherActor->IsA(AWarriorCharacter::StaticClass())) {
		return;
	}

	if(!m_actorsToDamage.ContainsByPredicate([&](const ActorToDamage& actorToDamage) { return actorToDamage.actor == OtherActor; })) {
		ActorToDamage atd;
		atd.actor = OtherActor;
		atd.timeCount = 0.0f;

		m_actorsToDamage.Add(atd);
	}
}

void AHealingDome::StopDamageEnemy(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if(OtherActor->IsA(AMageCharacter::StaticClass()) || OtherActor->IsA(AWarriorCharacter::StaticClass())) {
		return;
	}

	if(auto atd = m_actorsToDamage.FindByPredicate([&](const ActorToDamage& actorToDamage) { return actorToDamage.actor == OtherActor; })) {
		m_actorsToDamage.RemoveSingle(*atd);
	}
}

void AHealingDome::SetShouldntHeal()
{
	shouldntHeal = true;
}