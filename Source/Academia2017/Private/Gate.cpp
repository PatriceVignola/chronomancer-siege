#include "Academia2017.h"
#include "Gate.h"
#include "EventManager.h"

AGate::AGate()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGate::BeginPlay()
{
	Super::BeginPlay();
}

void AGate::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AGate::NotifyDoorFinishedClosing()
{
	if (HasAuthority())
	{
		OnDoorFinishedClosing.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("FINISHED CLOSING"));
	}
}