// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GETravelManagerBase.h"
/**
 *  Maps movement to a sphere
 */

class GALACTICENGAGEMENT_API GESphericalTravelManager : public GETravelManagerBase
{
private:
	float SphereRadius;
	FVector Origin;

private:
	void ProjectPointToSphere(FVector Point, FVector &ProjectedPoint, FVector& Up);

public:
	GESphericalTravelManager(FVector origin,float sphereRadius);
	virtual ~GESphericalTravelManager();
	
	virtual bool MoveShipToPoint(class AGEBaseShip* actor, FVector inDesiredPoint, FVector& outActualPoint, FHitResult& outHitResult)override;
	virtual bool MoveActorToPoint(class AActor* actor,FVector inDesiredPoint,FVector& outActualPoint, FHitResult& outHitResult)override;
	virtual float GetDistanceToActor(class AActor* actor1,class AActor* actor2)override;
	virtual FVector GetShortestDirectionTo(class AActor* actor1, class AActor* actor2)override;
};