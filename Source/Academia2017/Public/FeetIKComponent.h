// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "FeetIKComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UFeetIKComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, Category = IK, meta=(AllowPrivateAccess = "true"))
	FName LeftFootName;

	UPROPERTY(EditInstanceOnly, Category = IK, meta=(AllowPrivateAccess = "true"))
	FName RightFootName;

	UPROPERTY(BlueprintReadOnly, Category = IK, meta=(AllowPrivateAccess = "true"))
	float LeftFootEndEffector;

	UPROPERTY(BlueprintReadOnly, Category = IK, meta=(AllowPrivateAccess = "true"))
	float RightFootEndEffector;

public:	
	// Sets default values for this component's properties
	UFeetIKComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	USkeletalMeshComponent *mesh;
	
};
