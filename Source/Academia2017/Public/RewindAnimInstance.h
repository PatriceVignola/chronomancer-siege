// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "RewindAnimInstance.generated.h"

UCLASS()
class ACADEMIA2017_API URewindAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	URewindAnimInstance(const FObjectInitializer& ObjectInitializer);

	/*
	void ParallelEvaluateAnimation(bool bForceRefPose, const USkeletalMesh* InSkeletalMesh, TArray<FTransform>& OutBoneSpaceTransforms, FBlendedHeapCurve& OutCurve);
	void EvaluateAnimation(FPoseContext& Output);
	virtual bool NativeEvaluateAnimation(FPoseContext& Output) override;*/
};
