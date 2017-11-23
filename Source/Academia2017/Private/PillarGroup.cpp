// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "Engine.h"
#include "PillarGroup.h"
#include "Gate.h"
#include "Elevator.h"

// Sets default values
APillarGroup::APillarGroup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APillarGroup::BeginPlay()
{
	Super::BeginPlay();
	
	IsActive = false;
	IsActivated = false;
}

// Called every frame
void APillarGroup::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );


	for(int i = 0; i < Pillars.Num(); ++i) {
		if(Pillars[i]->State == PillarState::Activated) {
			IsActive = true;
			break;
		}
	}

	if(IsActive && !IsActivated) {
		bool allActivated = true;
		for(int i = 0; i < Pillars.Num(); ++i) {
			if(Pillars[i]->State == PillarState::Deactivated) {
				Pillars[i]->Active();
				allActivated = false;

				if(!m_wasGateActive) {
					m_wasGateActive = true;
					if(Gate) {
						Gate->SetActive();
					}
				}

				if(!m_wasElevatorActive) {
					m_wasElevatorActive = true;
					if(Elevator) {
						Elevator->SetActive();
					}
				}
			} else if(Pillars[i]->State == PillarState::Active) {
				allActivated = false;
			}
		}

		if(allActivated) {
			IsActivated = true;

			PlayCompletionSound();

			if(Gate && Gate->isOpenableByPillar()) {
				Gate->Open();
			}

			if(Elevator && Elevator->isOpenableByPillar()) {
				Elevator->Open();
			}

			if(Chronolith) {
				Chronolith->ActivateStunEffect();
			}

		} else if(ResetDelay >= 0.01f) {
			delay += DeltaTime;

			if(delay >= ResetDelay) {
				ResetPillars();
			}
		}
	}
}

void APillarGroup::ResetPillars() {
	if(HasAuthority()) {
		RPC_ResetPillars();
	}
}

void APillarGroup::RPC_ResetPillars_Implementation() 
{
	for(int i = 0; i < Pillars.Num(); ++i) {
		Pillars[i]->Deactivate();
	}

	if(Gate) {
		m_wasGateActive = false;
		Gate->Close();
		Gate->SetDeactivated();
	}

	if(Elevator) {
		m_wasElevatorActive = false;
		Elevator->Close();
		Elevator->SetDeactivated();
	}

	delay = 0.0f;
	IsActive = false;
	IsActivated = false;
}