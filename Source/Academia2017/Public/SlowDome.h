#pragma once

#include "GameFramework/Actor.h"
#include "SlowDome.generated.h"

UCLASS()
class ACADEMIA2017_API ASlowDome : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent *Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent *SlowZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent *SlowParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SlowDome, meta = (AllowPrivateAccess = "true"))
	float SlowPercent = 0.9;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SlowDome, meta = (AllowPrivateAccess = "true"))
	float DomeDuration = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SlowDome, meta = (AllowPrivateAccess = "true"))
	float DomeCooldown = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SlowDome, meta = (AllowPrivateAccess = "true"))
	float EnergyCost = 50;

public:	
	ASlowDome();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	float GetDomeCooldown();
	float GetEnergyCost();
	void StartLosingLife();

private:
	UFUNCTION()
	void StartSlowing(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void EndSlowing(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
