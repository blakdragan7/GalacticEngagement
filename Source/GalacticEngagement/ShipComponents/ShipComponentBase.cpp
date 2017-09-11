// Fill out your copyright notice in the Description page of Project Settings.

#include "ShipComponentBase.h"
#include "ComponentMountPoint.h"
#include "GEBaseShip.h"

bool UShipComponentBase::AssignToMountPoint(UComponentMountPoint * MountPoint)
{
	if(MountPoint == 0 )return false;
	MountedLocation = MountPoint;
	if (AGEBaseShip* ship = Cast<AGEBaseShip>(MountPoint->GetOwner()))
	{
		ControlledShip = ship;
	}
	return true;
}
