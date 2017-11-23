#pragma once

#include "Components/ActorComponent.h"
#include "SnapshotInfo.h"
#include "MagicHairComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACADEMIA2017_API UMagicHairComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMagicHairComponent();
	virtual void BeginPlay() override;

private:
	USkeletalMeshComponent *mesh = nullptr;

	void DeactivateHairPhysics(bool isFixedDuration, float duration);
	void ReactivateHairPhysics(const TArray<FSnapshotInfo> &);
};
