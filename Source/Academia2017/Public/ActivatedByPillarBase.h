#pragma once

#include "GameFramework/Actor.h"
#include "PillarGroup.h"
#include "ActivablePillar.h"
#include "ActivatedByPillarBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpenActivableRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCloseActivableRequestedDelegate);

UCLASS()
class ACADEMIA2017_API AActivatedByPillarBase : public AActor
{
	GENERATED_BODY()

public:	
	AActivatedByPillarBase();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = Activable)
	void Open();

	UFUNCTION(BlueprintCallable, Category = Activable)
	void Close();

	UFUNCTION(BlueprintCallable, Category = Activable)
	void SetActive();

	const bool isOpenableByPillar() const;

	UFUNCTION(BlueprintCallable, Category = Activable)
	void SetDeactivated();

protected:
	UPROPERTY(EditAnywhere)
	bool OpenableByPillar = false;

	UPROPERTY(EditAnywhere)
	bool InitiallyOpened = false;

	UPROPERTY(VisibleAnywhere)
	USceneComponent *Root;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent *Mesh;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOpenActivableRequestedDelegate OnOpenActivableRequested;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FCloseActivableRequestedDelegate OnCloseActivableRequested;

	/*UPROPERTY(EditInstanceOnly)
	APillarGroup* PillarGroup = nullptr;*/

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FPillarDeactivatedDelegate OnPillarDeactivated;
	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FPillarActiveDelegate OnPillarActive;
	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FPillarActivatedDelegate OnPillarActivated;

private:
	bool opened = false;
	bool savedGameState = false;

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Close();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Open();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_SetActive();

	void SaveActivableState();
	void LoadActivableState();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_LoadActivableState();

	void ResetAnimScale();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_ResetAnimScale();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_SetDeactivated();
};
