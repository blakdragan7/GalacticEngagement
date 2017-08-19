// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GEEngineBaseComponent.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API UGEEngineBaseComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
private:
	float CurrentSpeed;
	float CurrentRotationRate;

protected:
	UPROPERTY(Category = "Ship Movement", VisibleAnywhere, BlueprintReadOnly)
	bool IsMoving;
	UPROPERTY(Category = "Ship Movement", EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;
	UPROPERTY(Category = "Ship Movement", EditAnywhere, BlueprintReadWrite)
	float MaxAccel;
	UPROPERTY(Category = "Ship Movement", EditAnywhere, BlueprintReadWrite)
	float MaxRotationRate;
	UPROPERTY(Category = "Ship Movement", EditAnywhere, BlueprintReadWrite)
	float MaxRotationAccel;
	UPROPERTY(Category = "Ship Movement", EditAnywhere, BlueprintReadWrite)
	float RotationDeccel;
	UPROPERTY(Category = "Ship Movement", EditAnywhere, BlueprintReadWrite)
	float SpeedDeccel;

public:
	UGEEngineBaseComponent();
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)override;

	virtual void UpdateMovementRates(float DeltaTime);
	virtual void StopMoving();
	virtual void MoveTo(FVector WorldMoveToLocation);
};