#pragma once

#include "GameFramework/Character.h"
#include "PlayableCharacter.generated.h"

UCLASS()
class ACADEMIA2017_API APlayableCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage *DeathAnimation = nullptr;

public:
	APlayableCharacter();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlayDeathAnimation();

private:
	USkeletalMeshComponent *mesh = nullptr;
	UAnimInstance *animInstance = nullptr;
};
