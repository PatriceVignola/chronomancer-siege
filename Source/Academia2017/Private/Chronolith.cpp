// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "Engine.h"
#include "Chronolith.h"


// Sets default values
AChronolith::AChronolith()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AChronolith::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChronolith::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AChronolith::ActivateStunEffect() {
	if(HasAuthority()) {
		RPC_ActivateStunEffect();
	}
}

void AChronolith::RPC_ActivateStunEffect_Implementation() {
	BPActivateStunEffect();
}
