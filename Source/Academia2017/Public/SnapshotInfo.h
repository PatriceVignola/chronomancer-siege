#pragma once

#include "SnapshotInfo.generated.h"

USTRUCT(Atomic)
struct ACADEMIA2017_API FSnapshotInfo
{
	GENERATED_USTRUCT_BODY()

	FSnapshotInfo() {}
	FSnapshotInfo(FRotator rot, FVector pos, float time, bool canDamage, float Health) :
		rot(rot), pos(pos), time(time), CanDamage(canDamage), CurrentHealth(Health) {}

	FRotator rot;
	FVector pos;
	float time;
	bool CanDamage;
	float CurrentHealth;
};