#include "Academia2017.h"
#include "WarriorCharacter.h"
#include "Engine.h"
#include "PauseMenuSpawnerComponent.h"

UPauseMenuSpawnerComponent::UPauseMenuSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPauseMenuSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPauseMenuSpawnerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// Executed only once
	if (!inputComponent && GetOwner()->InputComponent)
	{
		inputComponent = GetOwner()->InputComponent;

		inputComponent->BindAction("PauseGame", IE_Pressed, this, &UPauseMenuSpawnerComponent::TogglePauseMenu);
		inputComponent->BindAxis("FocusNextOption", this, &UPauseMenuSpawnerComponent::nextOption);
		inputComponent->BindAxis("FocusPrevOption", this, &UPauseMenuSpawnerComponent::prevOption);
		inputComponent->BindAction("SelectMenuOption", IE_Pressed, this, &UPauseMenuSpawnerComponent::selectOption);
	}
}

void UPauseMenuSpawnerComponent::TogglePauseMenu()
{
	auto playableCharacter = Cast<APlayableCharacter>(GetOwner());

	if (playableCharacter) {
		auto pc = Cast<APlayerController>(playableCharacter->GetController());

		if (pc) {
			if (!pauseMenuVisible)
			{
				SpawnPauseMenu(pc);
				disableInput(pc);
			}
			else
			{
				Despawn(pc);
				enableInput(pc);
			}

			pauseMenuVisible = !pauseMenuVisible;
		}
	}	
}

void UPauseMenuSpawnerComponent::LoadMainMenu()
{
	if (GetOwner()->HasAuthority())
	{
		BackToMainMenu();
	}
	else
	{
		Server_RequestLoadMainMenu();
	}
}

void UPauseMenuSpawnerComponent::Server_RequestLoadMainMenu_Implementation()
{
	if (GetOwner()->HasAuthority())
	{
		LoadMainMenu();
	}
}

bool UPauseMenuSpawnerComponent::Server_RequestLoadMainMenu_Validate()
{
	return true;
}

void UPauseMenuSpawnerComponent::disableInput(APlayerController* controller) {
	if (controller) {
		controller->SetIgnoreMoveInput(true);
		controller->SetIgnoreLookInput(true);
	}
}

void UPauseMenuSpawnerComponent::enableInput(APlayerController* controller) {
	if (controller) {
		controller->SetIgnoreMoveInput(false);
		controller->SetIgnoreLookInput(false);
	}
}

void UPauseMenuSpawnerComponent::nextOption(float axisValue) {
	if(axisValue > 0.9 && pauseMenuVisible) {
		FocusNextOption();
		isQuitSelected = true;
	}
}

void UPauseMenuSpawnerComponent::prevOption(float axisValue) {
	if(axisValue > 0.9 && pauseMenuVisible) {
		FocusPrevOption();
		isQuitSelected = false;
	}
}

void UPauseMenuSpawnerComponent::selectOption() {
	if(pauseMenuVisible) {
		if(isQuitSelected) {
			SelectQuitAction();
			LoadMainMenu();
		} else {
			SelectResumeAction();
			TogglePauseMenu();
		}
	}
}
