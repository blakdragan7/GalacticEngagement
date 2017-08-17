// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextRenderActor.h"
#include "GEDamageIndicator.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEDamageIndicator : public ATextRenderActor
{
	GENERATED_BODY()
private:
	float TimeToLive;
	bool WasSetup;
	float speed;
	AGEDamageIndicator();

public:
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(meta = (WorldContext = "WorldContextObject"))
	static AGEDamageIndicator* SpawnIndicatorWithDamageAndDuration(UObject* WorldContextObject,FVector Location,float Duration,FString Damage);
};