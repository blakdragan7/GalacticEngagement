// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShipComponentBase.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "GEGunBaseComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GALACTICENGAGEMENT_API UGEGunBaseComponent : public UShipComponentBase
{
	GENERATED_BODY()
private:
	float ShotCountDown; // seconds until next shot
	bool ToggleShoot;
	bool CanShoot();

public:
	UGEGunBaseComponent();
	
	virtual void TickComponent(float DeltaTime)override;

	virtual bool FireGun(); // Fires gun using component rotaiton and location and direction
	virtual bool FireGun(FVector Direction); // Fires gun using component rotaiton and location
	virtual bool FireGun(FVector Direction, FVector& Location, FRotator &Rotation); // fires gun using given location and rotation

	UPROPERTY(Category = Ammo, EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AGEAmmoBase> AmmoType;
	UPROPERTY(Category = Control, EditAnywhere, BlueprintReadWrite)
	float ShotsPerSecond;
	UPROPERTY(Category = Stats, EditAnywhere, BlueprintReadWrite)
	float Damage;
};
