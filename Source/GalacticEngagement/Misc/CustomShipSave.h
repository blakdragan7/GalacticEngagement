// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ShipComponents/ShipComponentBase.h"
#include "CustomShipSave.generated.h"

USTRUCT()
struct FComponentSaveStruct
{
	GENERATED_BODY()
public:
	TSubclassOf<UShipComponentBase> componentClass;
	int32 index;
};

/**
 *  Save Game used for saving custom ships i.e. ships made in ship modification station
 */
UCLASS(Blueprintable, BlueprintType)
class GALACTICENGAGEMENT_API UCustomShipSave : public USaveGame
{
	GENERATED_BODY()
public:
	TArray<FComponentSaveStruct> PrimaryGunClasses;
	TArray<FComponentSaveStruct> SecondayGunClasses;
	
	FComponentSaveStruct EngineClass;
	FComponentSaveStruct ThrusterClass;

	UFUNCTION(BlueprintCallable, Category = Save)
	void PopulateFromShip(class AGEBaseShip* ship);
	UFUNCTION(BlueprintCallable,Category=Save)
	void PopulateShip(class AGEBaseShip* ship);
};
