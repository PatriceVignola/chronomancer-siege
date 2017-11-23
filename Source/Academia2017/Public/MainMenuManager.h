#pragma once

#include "GameFramework/Actor.h"
#include "NetworkManagerComponent.h"
#include "MainMenuManager.generated.h"

UCLASS()
class ACADEMIA2017_API AMainMenuManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMainMenuManager();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = MainMenu)
	void CreateLanGame();

	UFUNCTION(BlueprintCallable, Category = MainMenu)
	void JoinLanGame();

	UFUNCTION(BlueprintImplementableEvent, Category = MainMenu)
	void OnPlayerJoined(APlayerController *playerController);

	UFUNCTION(BlueprintCallable, Category = MainMenu)
	bool HasJoinedServer();

private:
	UNetworkManagerComponent *networkManager = nullptr;
	void NotifyPlayerJoined(APlayerController *playerController);
};
