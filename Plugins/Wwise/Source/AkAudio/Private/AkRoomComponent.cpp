// Copyright (c) 2006-2012 Audiokinetic Inc. / All Rights Reserved

/*=============================================================================
	AkRoomComponent.cpp:
=============================================================================*/

#include "AkRoomComponent.h"
#include "AkAudioDevice.h"
#include "AkAudioClasses.h"
#include "Net/UnrealNetwork.h"
#include "Components/BrushComponent.h"
#include "GameFramework/Volume.h"
#include "Model.h"
#include "EngineUtils.h"

/*------------------------------------------------------------------------------------
	UAkRoomComponent
------------------------------------------------------------------------------------*/

UAkRoomComponent::UAkRoomComponent(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	ParentVolume = NULL;

	// Property initialization
	NextLowerPriorityComponent = NULL;
	
	WallOcclusion = 1.0f;

	bEnable = true;
}

FName UAkRoomComponent::GetName() const
{
	return ParentVolume->GetFName();
}

bool UAkRoomComponent::HasEffectOnLocation(const FVector& Location) const
{
	// Need to add a small radius, because on the Mac, EncompassesPoint returns false if
	// Location is exactly equal to the Volume's location
	static float RADIUS = 0.01f;
	return RoomIsActive() && ParentVolume->EncompassesPoint(Location, RADIUS);
}

void UAkRoomComponent::PostLoad()
{
	Super::PostLoad();
	InitializeParentVolume();

	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice && RoomIsActive())
	{
		AkAudioDevice->AddRoomComponentToPrioritizedList(this);
	}
}

void UAkRoomComponent::BeginPlay()
{
	Super::BeginPlay();

	AddSpatialAudioRoom();
}

void UAkRoomComponent::InitializeParentVolume()
{
	ParentVolume = Cast<AVolume>(GetOwner());
	if (!ParentVolume)
	{
		bEnable = false;
		UE_LOG(LogAkAudio, Error, TEXT("UAkRoomComponent requires to be attached to an actor inheriting from AVolume."));
	}
}

void UAkRoomComponent::BeginDestroy()
{
	Super::BeginDestroy();
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice && RoomIsActive())
	{
		AkAudioDevice->RemoveRoomComponentFromPrioritizedList(this);
		RemoveSpatialAudioRoom();
	}
}

void UAkRoomComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice && RoomIsActive())
	{
		AkAudioDevice->RemoveRoomComponentFromPrioritizedList(this);
		RemoveSpatialAudioRoom();
	}
}

void UAkRoomComponent::AddSpatialAudioRoom()
{
	if(!RoomIsActive())
		return;

	TArray<AAkAcousticPortal*> IntersectingPortals;

	FString nameStr = ParentVolume->GetName();

	FRotator rotation = ParentVolume->GetActorRotation();

	FVector Front = rotation.RotateVector(FVector::RightVector);
	FVector Up = rotation.RotateVector(FVector::UpVector);

	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if(!AkAudioDevice)
		return;

	AkRoomParams params;
	AkAudioDevice->FVectorToAKVector(Front, params.Front);
	AkAudioDevice->FVectorToAKVector(Up, params.Up);
	params.strName = TCHAR_TO_ANSI(*nameStr);

	params.WallOcclusion = WallOcclusion;

	UAkLateReverbComponent* pRvbCmtp = (UAkLateReverbComponent*)ParentVolume->GetComponentByClass(UAkLateReverbComponent::StaticClass());
	if (pRvbCmtp)
	{
		params.ReverbAuxBus = pRvbCmtp->GetAuxBusId();
		params.ReverbLevel = pRvbCmtp->SendLevel;
	}

	AkAudioDevice->SetRoom(this, params);

	RoomAdded = true;
}

void UAkRoomComponent::RemoveSpatialAudioRoom()
{
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if(RoomIsActive() && AkAudioDevice)
		AkAudioDevice->RemoveRoom(this);

	RoomAdded = false;
}

#if WITH_EDITOR
void UAkRoomComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	InitializeParentVolume();
	
	//Call add again to update the room parameters, if it has already been added.
	if (RoomAdded)
		AddSpatialAudioRoom();
}
#endif
