// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "TutorialPromptSpawnerComponent.generated.h"

UENUM(BlueprintType)
enum class TutorialPrompt : uint8 {
	Kalika,
	Combo,
	KalikaCloneTime,
	Aion,
	CloneTime,
	SlowTime,
	Heal
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UTutorialPromptSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTutorialPromptSpawnerComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UFUNCTION(BlueprintCallable, Category = Tutorial)
	void SpawnNextPrompt(TutorialPrompt tutorial);

	UFUNCTION(BlueprintCallable, Category = Tutorial)
	void RemoveTutorialPrompt();

	UFUNCTION(BlueprintImplementableEvent, Category=Tutorial)
	void BPSpawnNextPrompt(TutorialPrompt Tutorial, APlayerController* playerController);

	UFUNCTION(BlueprintImplementableEvent, Category = Tutorial)
	void BPDespawn();

private:
	void disableInput();
	void enableInput();

	enum class OwnerType {
		Warrior,
		Mage,
		Unknown
	};

	OwnerType m_ownerType;
	UInputComponent *inputComponent = nullptr;
};
