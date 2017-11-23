#include "Academia2017.h"
#include "EventManager.h"
#include "MainMenuGameMode.h"


void AMainMenuGameMode::TravelToLevel(const FString &levelName)
{
	GetWorld()->ServerTravel(levelName);
}

void AMainMenuGameMode::PostLogin(APlayerController *playerController)
{
	Super::PostLogin(playerController);

	EventManager::OnPlayerJoined.Broadcast(playerController);
}