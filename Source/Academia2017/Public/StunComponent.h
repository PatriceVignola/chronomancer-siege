// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneComponent.h"
#include "PillarGroup.h"
#include "Net/UnrealNetwork.h"
#include "StunComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACADEMIA2017_API UStunComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStunComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float StunTime = 10.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Replicated)
	bool IsStunned = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	APillarGroup* PillarGroupActor;
private:
	float m_Timer = 0.0f;
};
