// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GEVelocityEffector.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UGEVelocityEffector : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GALACTICENGAGEMENT_API IGEVelocityEffector
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Velocity")
	FVector GetVelocityDeltaForPosition(FVector Position); // This is Velocity that should be added to current velocity
};
