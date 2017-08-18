// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GEBaseEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEBaseEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

	virtual void Possess(APawn* InPawn) override;
	virtual void UnPossess() override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Ship")
	class AGEBaseShip* ControlledShip;
};
