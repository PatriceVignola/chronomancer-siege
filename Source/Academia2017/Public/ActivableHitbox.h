#pragma once

#include "HitBoxComponent.h"
#include "ActivableHitbox.generated.h"

UCLASS()
class ACADEMIA2017_API UActivableHitbox : public UHitBoxComponent
{
	GENERATED_BODY()
public:	
	UActivableHitbox();
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool IsActivated = false;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FVector HitLocation = FVector(0, 0, 0);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Activate(FVector hitLoc);
};
