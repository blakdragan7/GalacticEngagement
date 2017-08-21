// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class GALACTICENGAGEMENT_API GEGameStatistics
{
public:
	static bool FindLookAtAngle2D(const FVector2D& Start, const FVector2D& Target, float &Angle);
	static bool VectorLessThenVector(FVector &v1, FVector &v2, float tolerance = 0.00001);
	static bool VectorGreaterThenVector(FVector &v1, FVector &v2,float tolerance = 0.00001);
	static bool VectorLessThenMagnitude(FVector &v1, float m, float tolerance = 0.00001);
	static bool VectorGreaterThenMagnitude(FVector &v1, float m, float tolerance = 0.00001);

};
