#pragma once

#include "GameFramework/Actor.h"
#include "NetworkManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientJoined);

UCLASS()
class ACADEMIA2017_API ANetworkManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ANetworkManager();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = Networking)
	bool ConnectedToServer();

	UPROPERTY(BlueprintAssignable, Category = Networking)
	FClientJoined OnClientJoined;

private:
	void NotifyClientJoined(APlayerController *playerController);
};
