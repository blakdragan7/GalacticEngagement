// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *  Plane for managing Background Actors
 */
class GALACTICENGAGEMENT_API FBackgroundPlane
{
private:
	float ActorDensity;
	float FarPlane; // Currently just z plane
	float WorldRadiusToScreenEdgeSqr;
	float WorldRadiusToScreenEdge;
	FVector PlaneNormal;
	FVector Origin;

	TArray<class AActor*> ActorsInBackground;
	TSubclassOf<AActor> PlaneAcotorClass;
private:
	void DestroyAllActors();

public:
	FBackgroundPlane();
	virtual ~FBackgroundPlane();

	// Wrap Actors If Outside Of Range
	virtual void UpdateActors();

	void SpawnAllActorsForBackground(UWorld* world);

	FORCEINLINE float GetRadiusToEdge()
	{
		return FMath::Sqrt(WorldRadiusToScreenEdgeSqr);
	}

	FORCEINLINE void SetActorClass(TSubclassOf<AActor> class_)
	{
		PlaneAcotorClass = class_;
	}

	FORCEINLINE void SetActorDensity(float density)
	{
		ActorDensity = density;
	}

	FORCEINLINE void SetFarPlane(float plane)
	{
		FarPlane = plane;
	}

	FORCEINLINE void SetRadiusToEdggeSqr(float radius)
	{
		WorldRadiusToScreenEdgeSqr = radius;
		WorldRadiusToScreenEdge = FMath::Sqrt(radius);
	}

	FORCEINLINE void SetPlaneNormal(FVector normal)
	{
		PlaneNormal = normal;
	}

	FORCEINLINE void SetOrigin(FVector org)
	{
		Origin = org;
	}

	FORCEINLINE TSubclassOf<AActor> GetActorClass()
	{
		return PlaneAcotorClass;
	}

	FORCEINLINE float GetFarPlane()
	{
		return FarPlane;
	}

	FORCEINLINE float GetActorDensity()
	{
		return ActorDensity;
	}

	FORCEINLINE float GetRadiusToEdggeSqr()
	{
		return WorldRadiusToScreenEdgeSqr;
	}

	FORCEINLINE FVector GetPlaneNormal()
	{
		return PlaneNormal;
	}

	FORCEINLINE FVector GetOrigin()
	{
		return Origin;
	}
	FORCEINLINE int32 GetNumActors()
	{
		return ActorsInBackground.Num();
	}
};
