// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "ShipComponents/ShipComponentBase.h"
#include "CustomShipSave.generated.h"

USTRUCT(BlueprintType)
struct FComponentSaveStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UShipComponentBase> componentClass;
	UPROPERTY(BlueprintReadWrite)
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Save)
	TArray<FComponentSaveStruct> PrimaryGunClasses;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Save)
	TArray<FComponentSaveStruct> SecondayGunClasses;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Save)
	FComponentSaveStruct EngineClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Save)
	FComponentSaveStruct ThrusterClass;

	/* Class Of Saved Ship or null if non saved */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Save)
	TSubclassOf<class AGEBaseShip> ShipClass;

	UFUNCTION(BlueprintCallable, Category = Save)
	void PopulateFromShip(class AGEBaseShip* ship);
	UFUNCTION(BlueprintCallable,Category=Save)
	void PopulateShip(class AGEBaseShip* ship);
};

FORCEINLINE FArchive &operator <<(FArchive &Ar, FComponentSaveStruct& TheStruct)
{
	Ar << TheStruct.componentClass;
	Ar << TheStruct.index;

	return Ar;
}
