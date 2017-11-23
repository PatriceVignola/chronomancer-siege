// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "WarriorCharacter.h"
#include "MageCharacter.h"
#include "Engine.h"
#include "TutorialPromptSpawnerComponent.h"


// Sets default values for this component's properties
UTutorialPromptSpawnerComponent::UTutorialPromptSpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTutorialPromptSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	auto owner = GetOwner();

	if(owner) {
		auto warriorCastOwner = Cast<AWarriorCharacter>(owner);

		if(warriorCastOwner) {
			m_ownerType = OwnerType::Warrior;
		} else {
			auto mageCastOwner = Cast<AMageCharacter>(owner);

			if(mageCastOwner) {
				m_ownerType = OwnerType::Mage;
			} else {
				m_ownerType = OwnerType::Unknown;
			}
		}
	}
}


// Called every frame
void UTutorialPromptSpawnerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if(!inputComponent && GetOwner()->InputComponent) {
		inputComponent = GetOwner()->InputComponent;

		inputComponent->BindAction("RemoveTutorialPrompt", IE_Pressed, this, &UTutorialPromptSpawnerComponent::RemoveTutorialPrompt);
	}
}

void UTutorialPromptSpawnerComponent::SpawnNextPrompt(TutorialPrompt tutorial) {
	disableInput();

	auto playableCharacter = Cast<APlayableCharacter>(GetOwner());

	if (playableCharacter) {
		auto pc = Cast<APlayerController>(playableCharacter->GetController());

		if (pc) {
			BPSpawnNextPrompt(tutorial, pc);
		}
		
	}
}

void UTutorialPromptSpawnerComponent::RemoveTutorialPrompt() {
	enableInput();
	BPDespawn();
}

void UTutorialPromptSpawnerComponent::disableInput() {
	if(m_ownerType == OwnerType::Warrior) {
		auto owner = Cast<AWarriorCharacter>(GetOwner());
		if(owner) {
			auto controller = Cast<APlayerController>(owner->GetController());
			if(controller) {
				controller->SetIgnoreMoveInput(true);
				controller->SetIgnoreLookInput(true);
			}
		}
	} else if(m_ownerType == OwnerType::Mage) {
		auto owner = Cast<AMageCharacter>(GetOwner());
		if(owner) {
			auto controller = Cast<APlayerController>(owner->GetController());
			if(controller) {
				controller->SetIgnoreMoveInput(true);
				controller->SetIgnoreLookInput(true);
			}
		}
	}
}

void UTutorialPromptSpawnerComponent::enableInput() {
	if(m_ownerType == OwnerType::Warrior) {
		auto owner = Cast<AWarriorCharacter>(GetOwner());
		if(owner) {
			auto controller = Cast<APlayerController>(owner->GetController());
			if(controller) {
				controller->SetIgnoreMoveInput(false);
				controller->SetIgnoreLookInput(false);
			}
		}
	} else if(m_ownerType == OwnerType::Mage) {
		auto owner = Cast<AMageCharacter>(GetOwner());
		if(owner) {
			auto controller = Cast<APlayerController>(owner->GetController());
			if(controller) {
				controller->SetIgnoreMoveInput(false);
				controller->SetIgnoreLookInput(false);
			}
		}
	}
}