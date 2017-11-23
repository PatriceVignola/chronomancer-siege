#include "Academia2017.h"
#include "MainMenuManager.h"
#include "EventManager.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"

AMainMenuManager::AMainMenuManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMainMenuManager::BeginPlay()
{
	Super::BeginPlay();

	if (AGameStateBase *gameState = GetWorld()->GetGameState())
	{
		networkManager = gameState->FindComponentByClass<UNetworkManagerComponent>();
	}

	EventManager::OnPlayerJoined.AddUObject(this, &AMainMenuManager::OnPlayerJoined);
}

void AMainMenuManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMainMenuManager::CreateLanGame()
{
	if (networkManager)
	{
		networkManager->CreateLanGame();
	}
}

void AMainMenuManager::JoinLanGame()
{
	if(networkManager)
	{
		networkManager->JoinLanGame();
	}
}

bool AMainMenuManager::HasJoinedServer()
{
	IOnlineSessionPtr Sessions = IOnlineSubsystem::Get()->GetSessionInterface();

	if (Sessions.IsValid())
	{
		FOnlineSessionSettings* CurrentSettings = Sessions->GetSessionSettings(GameSessionName);
		return !HasAuthority() && CurrentSettings != nullptr;
	}
	
	return false;
}