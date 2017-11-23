#pragma once

#include "Components/ActorComponent.h"
#include "PauseMenuSpawnerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpenPauseMenuRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClosePauseMenuRequestedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UPauseMenuSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FString MainMenuName = "PreLoadingScreen";

public:	
	UPauseMenuSpawnerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(BlueprintAssignable)
	FOpenPauseMenuRequestedDelegate OnOpenPauseMenuRequested;

	UPROPERTY(BlueprintAssignable)
	FClosePauseMenuRequestedDelegate OnClosePauseMenuRequested;

	UFUNCTION(BlueprintCallable, Category = PauseMenu)
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable, Category = PauseMenu)
	void TogglePauseMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = PauseMenu)
	void SpawnPauseMenu(APlayerController* playerController);

	UFUNCTION(BlueprintImplementableEvent, Category = PauseMenu)
	void Despawn(APlayerController* playerController);

	UFUNCTION(BlueprintImplementableEvent, Category = PauseMenu)
	void FocusNextOption();

	UFUNCTION(BlueprintImplementableEvent, Category = PauseMenu)
	void FocusPrevOption();

	UFUNCTION(BlueprintImplementableEvent, Category = PauseMenu)
	void SelectResumeAction();

	UFUNCTION(BlueprintImplementableEvent, Category = PauseMenu)
	void SelectQuitAction();

	UFUNCTION(BlueprintImplementableEvent, Category = PauseMenu)
	void BackToMainMenu();

private:
	void disableInput(APlayerController* controller);
	void enableInput(APlayerController* controller);

	void nextOption(float axisValue);
	void prevOption(float axisValue);
	void selectOption();

	UInputComponent *inputComponent = nullptr;
	bool pauseMenuVisible = false;

	bool isQuitSelected = false;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestLoadMainMenu();
};
