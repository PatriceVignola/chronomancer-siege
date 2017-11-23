#pragma once

#include "GameFramework/Actor.h"
#include "ActivablePillar.h"
#include "ActivatedByPillarBase.h"
#include "Gate.generated.h"

DECLARE_EVENT(AGate, FDoorFinishedClosing)

UCLASS()
class ACADEMIA2017_API AGate : public AActivatedByPillarBase
{
	GENERATED_BODY()

public:
	FDoorFinishedClosing OnDoorFinishedClosing;

	AGate();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

protected:
	UFUNCTION(BlueprintCallable, Category = Gate, meta = (AllowPrivateAccess = "true"))
	void NotifyDoorFinishedClosing();
};
