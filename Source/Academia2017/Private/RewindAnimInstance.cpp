// Fill out your copyright notice in the Description page of Project Settings.

#include "Academia2017.h"
#include "RewindAnimInstance.h"
#include "Runtime/Engine/Classes/Animation/AnimNodeBase.h"
#include "Runtime/Engine/Public/Animation/AnimInstanceProxy.h"


URewindAnimInstance::URewindAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
/*
void URewindAnimInstance::ParallelEvaluateAnimation(bool bForceRefPose, const USkeletalMesh* InSkeletalMesh, TArray<FTransform>& OutBoneSpaceTransforms, FBlendedHeapCurve& OutCurve)
{
	
}

void URewindAnimInstance::EvaluateAnimation(FPoseContext& Output)
{
	Super::EvaluateAnimation(Output);

	UE_LOG(LogTemp, Warning, TEXT("EvaluateAnimation"));
	for (auto abc : Output.Pose.GetBones())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *abc.GetRotation().Rotator().ToString());
		Output.ResetToAdditiveIdentity();
		abc.SetRotation(FQuat::Identity);
	}
}

bool URewindAnimInstance::NativeEvaluateAnimation(FPoseContext& Output)
{
	UE_LOG(LogTemp, Warning, TEXT("NativeEvaluateAnimation"));
	return true;
}*/