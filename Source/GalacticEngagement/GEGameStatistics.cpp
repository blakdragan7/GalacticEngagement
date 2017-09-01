// Fill out your copyright notice in the Description page of Project Settings.

#include "GEGameStatistics.h"

bool GEGameStatistics::FindLookAtAngle2D(const FVector2D& Start, const FVector2D& Target, float &Angle)
{
	FVector2D Normal = (Target - Start).GetSafeNormal();
	if (!Normal.IsNearlyZero())
	{
		Angle = FMath::RadiansToDegrees(FMath::Atan2(Normal.Y, Normal.X));
		return true;
	}
	return false;
}

bool GEGameStatistics::VectorLessThenVector(FVector & v1, FVector & v2, float tolerance)
{
	float v1m = v1.SizeSquared();
	float v2m = v2.SizeSquared();
	float dvm = v1m - v2m;
	return dvm < tolerance;
}

bool GEGameStatistics::VectorGreaterThenVector(FVector & v1, FVector & v2, float tolerance)
{
	return !VectorLessThenVector(v1,v2,-tolerance);
}

bool GEGameStatistics::VectorLessThenMagnitude(FVector & v1, float m, float tolerance)
{
	float v1m = v1.SizeSquared();
	float dvm = v1m - (m * m);
	return dvm < tolerance;
}

bool GEGameStatistics::VectorGreaterThenMagnitude(FVector & v1, float m, float tolerance)
{
	return !VectorLessThenMagnitude(v1,m,-tolerance);
}

const float GEGameStatistics::MaxVectorComponent(FVector v)
{
	return FMath::Max3<float>(v.X,v.Y,v.Z);
}

const float GEGameStatistics::MinVectorComponent(FVector v)
{
	return FMath::Min3<float>(v.X, v.Y, v.Z);
}

FMatrix GEGameStatistics::RotationMatrix(FVector center,FVector eye, FVector up)
{
	FVector Direction = (eye - center);
	return FRotationMatrix::MakeFromZX(up, Direction);
}

FQuat GEGameStatistics::QuatFromDirAndUp(FVector center, FVector eye, FVector up)
{
	return RotationMatrix(center, eye, up).ToQuat();
}

FRotator GEGameStatistics::RotatorFromDirAndUp(FVector center, FVector eye, FVector up)
{
	return RotationMatrix(center, eye, up).Rotator();
}
