#include "Academia2017.h"
#include "EventManager.h"
#include "NetworkManagerComponent.h"

UNetworkManagerComponent::UNetworkManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetworkManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	onlineSubsystem = IOnlineSubsystem::Get();

	if (onlineSubsystem)
	{
		sessionInterface = onlineSubsystem->GetSessionInterface();
	}

	EventManager::OnPlayerJoined.AddUObject(this, &UNetworkManagerComponent::NotifyPlayerJoined);

	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UNetworkManagerComponent::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UNetworkManagerComponent::OnStartSessionComplete);
}

void UNetworkManagerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UNetworkManagerComponent::CreateLanGame()
{
	// Creating a lan game from C++ seems to be buggy on 4.14.3, so we do it from the blueprint instead
	OnCreateLANGameRequested.Broadcast();

	/*
	if (sessionInterface.IsValid())
	{
		TSharedPtr<class FOnlineSessionSettings> sessionSettings = MakeShareable(new FOnlineSessionSettings());

		TSharedPtr<const FUniqueNetId> netID = GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId();

		if (sessionInterface->GetNamedSession("Game"))
		{
			sessionInterface->DestroySession("Game");
		}

		OnCreateSessionCompleteDelegateHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
		sessionCreated = sessionInterface->CreateSession(*netID, "Game", *sessionSettings);

		if (sessionCreated)
		{
			UE_LOG(LogTemp, Warning, TEXT("Session successfully created!"));
		}
	}*/
}

void UNetworkManagerComponent::JoinLanGame()
{
	OnJoinLANGameRequested.Broadcast();

	/*
	if (sessionInterface.IsValid())
	{
		sessionSearch = MakeShareable(new FOnlineSessionSearch());
		sessionSearch->bIsLanQuery = true;

		TSharedRef<FOnlineSessionSearch> sessionSearchRef = sessionSearch.ToSharedRef();

		OnFindSessionsComplete = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UNetworkManagerComponent::ShowSessions);
		OnFindSessionsCompleteHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsComplete);
		
		TSharedPtr<const FUniqueNetId> netID = GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId();

		if (netID.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("VALID NETID"));
		}

		sessionInterface->FindSessions(1, sessionSearchRef);

		//sessionInterface->finds(netID, searchSettings);
		//FOnlineSessionSearchResult sessionSearchResult;
		//sessionInterface->JoinSession(netID, sessionPrefix, )
	}*/
}

void UNetworkManagerComponent::NotifyPlayerJoined(APlayerController *newPlayerController)
{
	UE_LOG(LogTemp, Warning, TEXT("PLAYER JOINED!!!"));
}

void UNetworkManagerComponent::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete: %d"), bWasSuccessful);

	if (sessionInterface.IsValid())
	{
		// Clear the SessionComplete delegate handle, since we finished this call
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

		if (bWasSuccessful)
		{
			// Set the StartSession delegate handle
			OnStartSessionCompleteDelegateHandle = sessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

			// Our StartSessionComplete delegate should get called after this
			UE_LOG(LogTemp, Warning, TEXT("Starting %s"), *SessionName.ToString());
			sessionInterface->StartSession(SessionName);
		}
	}
}

void UNetworkManagerComponent::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnStartSessionComplete: %d"), bWasSuccessful);

	/*
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "00101_CenturianMineTemple", true, "listen");
	}*/
}

void UNetworkManagerComponent::ShowSessions(bool successful)
{
	UE_LOG(LogTemp, Warning, TEXT("WAS SUCCESSFUL: %d"), successful);

	//TSharedPtr<const FUniqueNetId> netID = GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId();
	FUniqueNetIdRepl netID = GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId();
	if (sessionInterface.IsValid())
	{
		// Clear the Delegate handle, since we finished this call
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);

		// Just debugging the Number of Search results. Can be displayed in UMG or something later on
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), sessionSearch->SearchResults.Num()));

		UE_LOG(LogTemp, Warning, TEXT("NUM: %d"), sessionSearch->SearchResults.Num());

		// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
		if (sessionSearch->SearchResults.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("NUM: %d"), sessionSearch->SearchResults.Num());
			// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
			// This can be customized later on with your own classes to add more information that can be set and displayed
			for (int i = 0; i < sessionSearch->SearchResults.Num(); i++)
			{
				sessionInterface->DestroySession("Game");
				sessionInterface->JoinSession(*netID, "Game", sessionSearch->SearchResults[i]);
				//sessionSearch->SearchResults[i]
				// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
				// This is something you can't do in Blueprint for example!
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), SearchIdx+1, *(sessionSearch->SearchResults[SearchIdx].Session.OwningUserName)));
			}
		}
	}
}