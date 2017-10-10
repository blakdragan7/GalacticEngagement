// Fill out your copyright notice in the Description page of Project Settings.

#include "GEGameStatics.h"
#include "Misc/CustomShipSave.h"
#include "ShipComponents/ComponentMountPoint.h"
#include "GEBaseShip.h"
#include "Runtime/Engine/Classes/Components/SceneComponent.h"

bool GEGameStatics::FindLookAtAngle2D(const FVector2D& Start, const FVector2D& Target, float &Angle)
{
	FVector2D Normal = (Target - Start).GetSafeNormal();
	if (!Normal.IsNearlyZero())
	{
		Angle = FMath::RadiansToDegrees(FMath::Atan2(Normal.Y, Normal.X));
		return true;
	}
	return false;
}

bool GEGameStatics::VectorLessThenVector(FVector & v1, FVector & v2, float tolerance)
{
	float v1m = v1.SizeSquared();
	float v2m = v2.SizeSquared();
	float dvm = v1m - v2m;
	return dvm < tolerance;
}

bool GEGameStatics::VectorGreaterThenVector(FVector & v1, FVector & v2, float tolerance)
{
	return !VectorLessThenVector(v1,v2,-tolerance);
}

bool GEGameStatics::VectorLessThenMagnitude(FVector & v1, float m, float tolerance)
{
	float v1m = v1.SizeSquared();
	float dvm = v1m - (m * m);
	return dvm < tolerance;
}

bool GEGameStatics::VectorGreaterThenMagnitude(FVector & v1, float m, float tolerance)
{
	return !VectorLessThenMagnitude(v1,m,-tolerance);
}

const float GEGameStatics::VectorPercentageOfMag(FVector & vector,float mag)
{
	if (vector.IsNearlyZero()) return 0;
	return vector.SizeSquared() / (mag*mag);
}

const float GEGameStatics::MaxVectorComponent(FVector &v)
{
	return FMath::Max3<float>(v.X,v.Y,v.Z);
}

const float GEGameStatics::MinVectorComponent(FVector &v)
{
	return FMath::Min3<float>(v.X, v.Y, v.Z);
}

FMatrix GEGameStatics::RotationMatrix(FVector& center,FVector& eye, FVector& up)
{
	FVector Direction = (eye - center);
	return FRotationMatrix::MakeFromZX(up, Direction);
}

FQuat GEGameStatics::QuatFromDirAndUp(FVector& center, FVector& eye, FVector& up)
{
	return RotationMatrix(center, eye, up).ToQuat();
}

FRotator GEGameStatics::RotatorFromDirAndUp(FVector& center, FVector& eye, FVector& up)
{
	return RotationMatrix(center, eye, up).Rotator();
}

bool GEGameStatics::PopulateShipFromNetStruct(AGEBaseShip * ship, FNetComponentSaveStruct & netStr)
{

	if (netStr.EngineClass.componentClass)
	{
		ship->EngineMount->AssignShipComponent(netStr.EngineClass.componentClass);
	}

	if (netStr.ThrusterClass.componentClass)
	{
		ship->ThrusterMount->AssignShipComponent(netStr.ThrusterClass.componentClass);
	}

	for (FComponentSaveStruct strComp : netStr.PrimaryGunClasses)
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

	for (FComponentSaveStruct strComp : netStr.SecondayGunClasses)
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

	return true;
}