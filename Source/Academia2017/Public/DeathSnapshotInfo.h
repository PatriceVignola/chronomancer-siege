#pragma once

#include "DeathSnapshotInfo.generated.h"

USTRUCT(Atomic)
struct ACADEMIA2017_API FDeathSnapshotInfo
{
	GENERATED_USTRUCT_BODY()

	FDeathSnapshotInfo() {}

	FDeathSnapshotInfo(FRotator rot, FVector pos, float time, float Health) :
		rot(rot), pos(pos), time(time), CurrentHealth(Health) {}

	FRotator rot;
	FVector pos;
	float time;
	float CurrentHealth;
};