#pragma once

#include "Components/ActorComponent.h"
#include "SnapshotInfo.h"
#include "TimeEventManagerComponent.h"
#include "DamageableCharacterComponent.h"
#include "RewindManagerComponent.h"
#include "RewindableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnableRewindPostProcessRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisableRewindPostProcessRequested);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACADEMIA2017_API URewindableComponent : public UActorComponent {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Time, meta = (AllowPrivateAccess = "true"))
	bool rewinding = false;

public:
	URewindableComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Rewind(const TArray<FSnapshotInfo> &snapshotStack, bool destroyAfterRewind);

	UPROPERTY(BlueprintAssignable, Category = Record)
	FEnableRewindPostProcessRequested OnEnableRewindPostProcessRequested;

	UPROPERTY(BlueprintAssignable, Category = Record)
	FDisableRewindPostProcessRequested OnDisableRewindPostProcessRequested;

private:
	UTimeEventManagerComponent *timeEventManager = nullptr;;
	URewindManagerComponent *rewindManager = nullptr;
	UDamageableCharacterComponent* damageableComponent = nullptr;
	TArray<FSnapshotInfo> rewindStack;
	TArray<FSnapshotInfo> replayStack;
	float relativeTime = 0.f;
	bool shouldDestroyAfterRewind = false;

	void StopRewind();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StopRewind();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_DisableInput();

	void CancelRewinding();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_CancelRewinding();
};
