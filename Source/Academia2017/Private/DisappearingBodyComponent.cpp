#include "Academia2017.h"
#include "TimeEventManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "DisappearingBodyComponent.h"

UDisappearingBodyComponent::UDisappearingBodyComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bReplicates = true;
}

void UDisappearingBodyComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		if (UTimeEventManagerComponent *timeEventManager = GetOwner()->FindComponentByClass<UTimeEventManagerComponent>())
		{
			timeEventManager->OnCharacterDied.AddUObject(this, &UDisappearingBodyComponent::NoticeCharacterDiedState);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
			timeEventManager->OnRecordStarted.AddUObject(this, &UDisappearingBodyComponent::NoticeRecordStarted);
			timeEventManager->OnRecordStopped.AddUObject(this, &UDisappearingBodyComponent::NoticeRecordStopped);
		}
	}

	CacheDynamicMaterials();
}

void UDisappearingBodyComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDisappearingBodyComponent, disappearingTimer);
}

void UDisappearingBodyComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (GetOwner()->HasAuthority() && deathPending)
	{
		disappearingTimer += DeltaTime;
		OnTimeStampReplicated();

		if (disappearingTimer >= TimeBeforeDisappearing + FadingDuration)
		{
			GetOwner()->Destroy();
			deathPending = false;
		}
	}
}

void UDisappearingBodyComponent::NoticeRecordStarted()
{
	if (GetOwner()->HasAuthority())
	{
		recording = true;
	}
}

void UDisappearingBodyComponent::NoticeRecordStopped(const TArray<FSnapshotInfo> &, bool)
{
	if (GetOwner()->HasAuthority())
	{
		recording = false;
	}
}

void UDisappearingBodyComponent::NoticeCharacterDiedState()
{
	if(GetOwner()->HasAuthority() && !recording && !deathPending)
	{
		deathPending = true;
		disappearingTimer = 0.f;
	}
}

void UDisappearingBodyComponent::CacheDynamicMaterials()
{
	TArray<UActorComponent *> primitives = GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass());

	for (UActorComponent *comp : primitives)
	{
		UPrimitiveComponent *primitive = Cast<UPrimitiveComponent>(comp);

		int materialsCount = primitive->GetNumMaterials();

		for (int i = 0; i < materialsCount; i++)
		{
			UMaterialInstanceDynamic *dynamicMaterial = primitive->CreateDynamicMaterialInstance(i);
			primitive->SetMaterial(i, dynamicMaterial);
			dynamicMaterials.Add(dynamicMaterial);
		}
	}
}

void UDisappearingBodyComponent::SetMaterialsAlpha(float newAlpha)
{
	for (UMaterialInstanceDynamic *dynamicMaterial : dynamicMaterials)
	{
		if (dynamicMaterial)
		{
			dynamicMaterial->SetScalarParameterValue("Alpha", newAlpha);
		}
	}
}

void UDisappearingBodyComponent::OnTimeStampReplicated()
{
	SetMaterialsAlpha(FMath::Lerp(1.f, 0.f, (disappearingTimer - TimeBeforeDisappearing) / FadingDuration));
}