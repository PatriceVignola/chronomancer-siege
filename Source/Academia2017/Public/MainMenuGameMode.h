#pragma once

#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

UCLASS()
class ACADEMIA2017_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Multiplayer)
	void TravelToLevel(const FString &levelName);
	virtual void PostLogin(APlayerController *playerController) override;
};
