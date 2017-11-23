#pragma once

#include "ClothSnapshotInfo.generated.h"

USTRUCT(Atomic)
struct ACADEMIA2017_API FClothSnapshotInfo
{
	GENERATED_USTRUCT_BODY()

	FClothSnapshotInfo() {}

	FClothSnapshotInfo(const TArray<FVector> &positions, float time) :
		positions(positions), time(time) {}

	TArray<FVector> positions;
	float time;
};