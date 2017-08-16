// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GEGunBaseComponent.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API UGEGunBaseComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
private:
	float ShotCountDown; // seconds until next shot
	bool ToggleShoot;
	
	bool CanShoot();

public:
	UGEGunBaseComponent();
	
	virtual void TickComponent(float DeltaTime,enum ELevelTick TickType,FActorComponentTickFunction * ThisTickFunction)override;

	virtual bool FireGun(); // Fires gun using component rotaiton and location and direction
	virtual bool FireGun(FVector Direction); // Fires gun using component rotaiton and location
	virtual bool FireGun(FVector Direction, FVector& Location, FRotator &Rotation); // fires gun using given location and rotation

	UPROPERTY(Category = Ammo, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AGEAmmoBase> AmmoType;
	UPROPERTY(Category = Control, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ShotsPerSecond;;
};
