// Fill out your copyright notice in the Description page of Project Settings.

#include "GEGunBaseComponent.h"
#include "ComponentMountPoint.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "GEAmmoBase.h"

UGEGunBaseComponent::UGEGunBaseComponent()
{
	AmmoType = AGEAmmoBase::StaticClass();
	ShotCountDown = 0.0f;
	ShotsPerSecond = 3.0f;
	ToggleShoot = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshCube(TEXT("StaticMesh'/Game/Models/BaseShipSGun.BaseShipSGun'"));
	if (StaticMeshCube.Object)ComponentModel = StaticMeshCube.Object;
}

void UGEGunBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType)
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
		const FVector Location = MountedLocation->GetComponentLocation();
		const FRotator Rotation = MountedLocation->GetComponentRotation();
		const FVector Direction = MountedLocation->GetForwardVector();
		AGEAmmoBase* ammo = static_cast<AGEAmmoBase*>(GetWorld()->SpawnActor(AmmoType, &Location, &Rotation, spawnParams));
		ammo->Launch(MountedLocation->GetOwner(), Direction*100.0);
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
		const FVector Location = MountedLocation->GetComponentLocation();
		const FRotator Rotation = MountedLocation->GetComponentRotation();
		AGEAmmoBase* ammo = static_cast<AGEAmmoBase*>(GetWorld()->SpawnActor(AmmoType, &Location, &Rotation, spawnParams));
		if(ammo)ammo->Launch(MountedLocation->GetOwner(),Direction);
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
		ammo->Launch(MountedLocation->GetOwner(), Direction*2000.0);
		ToggleShoot = true;
		return true;
	}
	return false;
}

bool UGEGunBaseComponent::CanShoot()
{
	return ShotCountDown <= 0 && MountedLocation;
}
