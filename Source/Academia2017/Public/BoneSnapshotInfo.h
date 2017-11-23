#pragma once

#include "BoneSnapshotInfo.generated.h"

USTRUCT(Atomic)
struct ACADEMIA2017_API FBoneSnapshotInfo
{
	GENERATED_USTRUCT_BODY()
		
	FBoneSnapshotInfo() {}

	FBoneSnapshotInfo(const TArray<FTransform> &transforms, float time, int attackType, bool leftFootstep, bool rightFootstep) :
		transforms(transforms), time(time), attackType(attackType), leftFootstep(leftFootstep), rightFootstep(rightFootstep) {}

	TArray<FTransform> transforms;
	float time;

	// -1 = None
	int attackType;
	bool leftFootstep;
	bool rightFootstep;
};