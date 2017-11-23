#include "Academia2017.h"
#include "EventManager.h"
#include "NetworkManager.h"

ANetworkManager::ANetworkManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ANetworkManager::BeginPlay()
{
	Super::BeginPlay();

	EventManager::OnPlayerJoined.AddUObject(this, &ANetworkManager::NotifyClientJoined);
}

void ANetworkManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

bool ANetworkManager::ConnectedToServer()
{
	IOnlineSessionPtr Sessions = IOnlineSubsystem::Get()->GetSessionInterface();

	if (Sessions.IsValid())
	{
		FOnlineSessionSettings* CurrentSettings = Sessions->GetSessionSettings(GameSessionName);

		UE_LOG(LogTemp, Warning, TEXT("Sessions.IsValid()"));
		UE_LOG(LogTemp, Warning, TEXT("!HasAuthority() \t %d"), !HasAuthority());
		UE_LOG(LogTemp, Warning, TEXT("CurrentSettings != nullptr \t %d"), CurrentSettings != nullptr);

		return !HasAuthority() && CurrentSettings != nullptr;
	}

	return false;
}

void ANetworkManager::NotifyClientJoined(APlayerController *playerController)
{
	if (HasAuthority())
	{
		OnClientJoined.Broadcast();
	}
}