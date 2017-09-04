// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
/**
 *  Controls Movement For actors to fit specific criteria
 */

class GALACTICENGAGEMENT_API GETravelManagerBase
{
public:
	GETravelManagerBase();
	virtual ~GETravelManagerBase();
	
	virtual bool MoveShipToPoint(class AGEBaseShip* actor, FVector inDesiredPoint, FVector& outActualPoint, FHitResult& outHitResult) = 0;
	virtual bool MoveActorToPoint(class AActor* actor,FVector inDesiredPoint,FVector& outActualPoint, FHitResult& outHitResult)=0;
	virtual float GetDistanceToActor(class AActor* actor1,class AActor* actor2)=0;
	virtual FVector GetShortestDirectionTo(class AActor* actor1, class AActor* actor2)=0;
	static GETravelManagerBase* GetDefaultManager();
};