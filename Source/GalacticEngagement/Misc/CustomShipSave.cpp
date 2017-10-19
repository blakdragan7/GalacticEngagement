// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomShipSave.h"

#include "GEBaseShip.h"
#include "ShipComponents/GEGunBaseComponent.h"
#include "ShipComponents/GEEngineBaseComponent.h"
#include "ShipComponents/GEThrusterBaseComponent.h"
#include "ShipComponents/ComponentMountPoint.h"
#include "Runtime/Engine/Classes/Engine/World.h"

void UCustomShipSave::PopulateFromShip(class AGEBaseShip* ship)
{
	if (UShipComponentBase * component = ship->EngineMount->GetComponentBase())
	{
		EngineClass.componentClass = component->GetClass();
		EngineClass.index = ship->EngineMount->index;
	}
	if (UShipComponentBase * component = ship->ThrusterMount->GetComponentBase())
	{
		ThrusterClass.componentClass = component->GetClass();
		ThrusterClass.index = ship->ThrusterMount->index;
	}
	for (UComponentMountPoint* mount : ship->MainGunComponents)
	{
		if (UShipComponentBase * component = mount->GetComponentBase())
		{
			FComponentSaveStruct strComp;
			strComp.componentClass = component->GetClass();
			strComp.index = mount->index;
			PrimaryGunClasses.Add(strComp);
		}
	}

	for (UComponentMountPoint* mount : ship->SecondaryGunComponents)
	{
		if (UShipComponentBase * component = mount->GetComponentBase())
		{
			FComponentSaveStruct strComp;
			strComp.componentClass = component->GetClass();
			strComp.index = mount->index;
			SecondayGunClasses.Add(strComp);
		}
	}

	ShipClass = ship->GetClass();
}

void UCustomShipSave::PopulateShip(AGEBaseShip * ship)
{
	if (IsValid(ship) == false)
	{
		UE_LOG(LogTemp,Error,TEXT("Trying To Populate Invalid Ship !"));
		return;
	}
	if (EngineClass.componentClass)
	{
		ship->EngineMount->AssignShipComponent(EngineClass.componentClass);
	}

	if (ThrusterClass.componentClass)
	{
		ship->ThrusterMount->AssignShipComponent(ThrusterClass.componentClass);
	}

	for (FComponentSaveStruct strComp : PrimaryGunClasses)
	{
		if (strComp.componentClass && strComp.index != -1)
		{
			for (UComponentMountPoint* mount : ship->MainGunComponents)
			{
				if (mount->index == strComp.index)
				{
					mount->AssignShipComponent(strComp.componentClass);
					break;
				}
			}
		}
	}

	for (FComponentSaveStruct strComp : SecondayGunClasses)
	{
		if (strComp.componentClass && strComp.index != -1)
		{
			for (UComponentMountPoint* mount : ship->SecondaryGunComponents)
			{
				if (mount->index == strComp.index)
				{
					mount->AssignShipComponent(strComp.componentClass);
					break;
				}
			}
		}
	}
}

bool FComponentSaveStruct::NetSerialize(FArchive & Ar, UPackageMap * Map, bool & bOutSuccess)
{
	Ar << *this;
	bOutSuccess = true;
	return true;
}