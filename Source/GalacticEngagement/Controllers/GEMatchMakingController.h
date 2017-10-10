// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameFramework/PlayerController.h"
#include "GEMatchMakingController.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEMatchMakingController : public APlayerController
{
	GENERATED_BODY()
private:

public:
	AGEMatchMakingController();

	virtual void BeginPlay()override;

	UFUNCTION(NetMulticast, Reliable)
	void MultiNewPlayerLogin();

	UFUNCTION(BlueprintImplementableEvent)
	void UINeedsUpdate();
};
