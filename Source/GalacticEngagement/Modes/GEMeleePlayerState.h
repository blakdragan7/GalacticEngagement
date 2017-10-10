// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/CustomShipSave.h"
#include "GameFramework/PlayerState.h"
#include "GEMeleePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEMeleePlayerState : public APlayerState
{
	GENERATED_BODY()
private:
	bool HasSpawnedShip;

public:
	AGEMeleePlayerState();

	
};
