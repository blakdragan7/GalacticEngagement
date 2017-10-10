// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "GameFramework/SaveGame.h"
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

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

USTRUCT(BlueprintType)
struct FNetComponentSaveStruct
{
	GENERATED_BODY()
public:
	TArray<FComponentSaveStruct> PrimaryGunClasses;
	TArray<FComponentSaveStruct> SecondayGunClasses;

	FComponentSaveStruct EngineClass;
	FComponentSaveStruct ThrusterClass;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	TSubclassOf<class AGEBaseShip> ShipClass;
};

template<>
struct TStructOpsTypeTraits<FNetComponentSaveStruct> : public TStructOpsTypeTraitsBase2<FNetComponentSaveStruct>
{
	enum
	{
		WithNetSerializer = true
	};
};

template<>
struct TStructOpsTypeTraits<FComponentSaveStruct> : public TStructOpsTypeTraitsBase2<FComponentSaveStruct>
{
	enum
	{
		WithNetSerializer = true
	};
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

FORCEINLINE FArchive &operator <<(FArchive &Ar, FNetComponentSaveStruct& TheStruct)
{
	Ar << TheStruct.EngineClass;
	Ar << TheStruct.ThrusterClass;
	Ar << TheStruct.ShipClass;

	for (auto str : TheStruct.PrimaryGunClasses)
	{
		Ar << str;
	}

	for (auto str : TheStruct.SecondayGunClasses)
	{
		Ar << str;
	}


	return Ar;
}