#pragma once

#include "Components/ActorComponent.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "Runtime/Engine/Classes/Engine/LocalPlayer.h"
#include "NetworkManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerJoinedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCreateLANGameRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJoinLANGameRequestedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UNetworkManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNetworkManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void CreateLanGame();
	void JoinLanGame();

	UPROPERTY(BlueprintAssignable)
	FCreateLANGameRequestedDelegate OnCreateLANGameRequested;

	UPROPERTY(BlueprintAssignable)
	FJoinLANGameRequestedDelegate OnJoinLANGameRequested;

	UPROPERTY(BlueprintAssignable)
	FPlayerJoinedDelegate OnPlayerJoined;

private:
	// Create session
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;

	// Start session
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	
	// Find sessions
	FOnFindSessionsCompleteDelegate OnFindSessionsComplete;
	FDelegateHandle OnFindSessionsCompleteHandle;

	TSharedPtr<class FOnlineSessionSearch> sessionSearch;

	IOnlineSubsystem *onlineSubsystem = nullptr;
	IOnlineSessionPtr sessionInterface;
	APlayerController *playerController = nullptr;
	bool sessionCreated = false;
	FName sessionPrefix = "ChronomancerSiege";

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void NotifyPlayerJoined(APlayerController *playerController);
	void ShowSessions(bool successful);
};
