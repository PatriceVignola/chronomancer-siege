#include "Academia2017.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "PlayableCharacter.h"

APlayableCharacter::APlayableCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();

	mesh = FindComponentByClass<USkeletalMeshComponent>();

	if (mesh)
	{
		animInstance = mesh->GetAnimInstance();
	}
}

void APlayableCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayableCharacter::PlayDeathAnimation()
{
	if (animInstance && DeathAnimation)
	{
		animInstance->Montage_Play(DeathAnimation);
	}
}