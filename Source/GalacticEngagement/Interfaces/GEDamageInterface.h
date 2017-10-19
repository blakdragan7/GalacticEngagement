// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Interface.h"
#include "GEDamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGEDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GALACTICENGAGEMENT_API IGEDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ReceiveDamage(AActor* attacker,int32 Damage,FVector DamageLocation)=0;
	virtual int32 GetHealth()=0;
	
};
