// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Interface.h"
#include "GEPlanetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGEPlanetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GALACTICENGAGEMENT_API IGEPlanetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Size")
	void SetRadius(float radius);
	
};
