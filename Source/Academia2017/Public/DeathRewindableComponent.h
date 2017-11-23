#pragma once

#include "Components/ActorComponent.h"
#include "DeathSnapshotInfo.h"
#include "TimeEventManagerComponent.h"
#include "DamageableCharacterComponent.h"
#include "DeathRewindManagerComponent.h"
#include "DeathRewindableComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACADEMIA2017_API UDeathRewindableComponent : public UActorComponent {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Time, meta = (AllowPrivateAccess = "true"))
	bool rewinding = false;

public:
	UDeathRewindableComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = Record)
	bool IsRewinding();

private:
	UTimeEventManagerComponent *timeEventManager = nullptr;;
	UDeathRewindManagerComponent *rewindManager = nullptr;
	UDamageableCharacterComponent* damageableComponent = nullptr;
	TArray<FDeathSnapshotInfo> rewindStack;
	bool destroyAfterRewind = false;

	void Rewind(const TArray<FDeathSnapshotInfo> &snapshotStack);
	void StopRewind();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_StopRewind();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_DisableInput();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_EnableInput();
};
