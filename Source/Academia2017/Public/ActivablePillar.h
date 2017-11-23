#pragma once

#include "GameFramework/Actor.h"
#include "ActivableHitbox.h"
#include "ActivablePillar.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPillarDeactivatedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPillarActiveDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPillarActivatedDelegate);

UENUM(BlueprintType)
enum class PillarState : uint8 {
	Deactivated,
	Active,
	Activated
};

UCLASS()
class ACADEMIA2017_API AActivablePillar : public AActor
{
	GENERATED_BODY()
	
public:	
	AActivablePillar();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	float ResetDelay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	bool IsActivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UActivableHitbox* BodyHitBox;

	UFUNCTION(BlueprintCallable, Category = "Pillar")
	void Activate();

	UFUNCTION(BlueprintCallable, Category = "Pillar")
	void Deactivate();

	UFUNCTION(BlueprintCallable, Category = "Pillar")
	void Active();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pillar")
	void PillarActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pillar")
	void PillarActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pillar")
	void PillarDesactivated();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Enum)
	PillarState State;

private:
	float delay = 0.0f;

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PillarActivated();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PillarDesactivated();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_PillarActive();
};
