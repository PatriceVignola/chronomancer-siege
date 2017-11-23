#pragma once

#include "Components/ActorComponent.h"
#include "SnapshotInfo.h"
#include "DisappearingBodyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class ACADEMIA2017_API UDisappearingBodyComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float TimeBeforeDisappearing = 3.f;

	UPROPERTY(EditDefaultsOnly)
	float FadingDuration = 3.f;


public:
	UDisappearingBodyComponent();
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	TArray<UMaterialInstanceDynamic *> dynamicMaterials;
	bool recording = false;
	bool deathPending = false;

	UPROPERTY(ReplicatedUsing = OnTimeStampReplicated)
	float disappearingTimer = 0.f;

	void NoticeRecordStarted();
	void NoticeRecordStopped(const TArray<FSnapshotInfo> &, bool);
	void NoticeCharacterDiedState();
	void CacheDynamicMaterials();
	void SetMaterialsAlpha(float newAlpha);

	UFUNCTION()
	void OnTimeStampReplicated();
};
