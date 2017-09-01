// Fill out your copyright notice in the Description page of Project Settings.

#include "GEGunBaseComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "GEAmmoBase.h"

UGEGunBaseComponent::UGEGunBaseComponent() : UStaticMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	AmmoType = AGEAmmoBase::StaticClass();
	ShotCountDown = 0.0f;
	ShotsPerSecond = 3.0f;
	ToggleShoot = false;

	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshCube(TEXT("StaticMesh'/Game/Models/BaseShipSGun.BaseShipSGun'"));
	if (StaticMeshCube.Object)SetStaticMesh(StaticMeshCube.Object);
}

void UGEGunBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	if (ToggleShoot || ShotCountDown > 0) // Update ShotCountDown
	{
		if (ShotCountDown <= 0)
		{
			ShotCountDown = 1.0f / ShotsPerSecond;
			ToggleShoot = false;
		}
		else
		{
			ShotCountDown -= DeltaTime;
		}
	}
}

bool UGEGunBaseComponent::FireGun()
{
	if (CanShoot())
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		const FVector Location = GetComponentLocation();
		const FRotator Rotation = GetComponentRotation();
		const FVector Direction = GetForwardVector();
		AGEAmmoBase* ammo = static_cast<AGEAmmoBase*>(GetWorld()->SpawnActor(AmmoType, &Location, &Rotation, spawnParams));
		ammo->Launch(GetOwner(), Direction*100.0);
		ToggleShoot = true;
		return true;
	}
	return false;
}

bool UGEGunBaseComponent::FireGun(FVector Direction)
{
	if (CanShoot())
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FTransform transform;
		const FVector Location = GetComponentLocation();
		const FRotator Rotation = GetComponentRotation();
		AGEAmmoBase* ammo = static_cast<AGEAmmoBase*>(GetWorld()->SpawnActor(AmmoType, &Location, &Rotation, spawnParams));
		if(ammo)ammo->Launch(GetOwner(),Direction);
		ToggleShoot = true;
		return true;
	}
	return false;
}

bool UGEGunBaseComponent::FireGun(FVector Direction,FVector & Location, FRotator &Rotation)
{
	if (CanShoot())
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AGEAmmoBase* ammo = static_cast<AGEAmmoBase*>(GetWorld()->SpawnActor(AmmoType, &Location, &Rotation, spawnParams));
		ammo->Launch(GetOwner(), Direction*2000.0);
		ToggleShoot = true;
		return true;
	}
	return false;
}

bool UGEGunBaseComponent::CanShoot()
{
	return ShotCountDown <= 0;
}
