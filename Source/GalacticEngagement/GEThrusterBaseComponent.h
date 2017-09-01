// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GEThrusterBaseComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GALACTICENGAGEMENT_API UGEThrusterBaseComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
private:
	FVector WorldMoveToLocation;
	FVector CurrentVelocity;
	FVector ExternalForce;
	FRotator DesiredRotation;
	float CurrentRotationRate;
	bool NeedsDirectionUpdate;
	bool HitLastFrame;

protected:
	UPROPERTY(Category = "Ship Movement", VisibleAnywhere, BlueprintReadOnly)
		class AGEBaseShip *controlledShip;
	UPROPERTY(Category = "Ship Movement", VisibleAnywhere, BlueprintReadOnly)
		bool IsAccelerating;
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
	UGEThrusterBaseComponent();
	/* Must be called within world context ex not in constructor */
	void SetControlledShip(class AGEBaseShip* ship);

	const FVector GetMoveToLocation()const;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)override;

	virtual void UpdateMovementRates(FVector Direction, float DeltaTime);
	virtual void StopMoving();
	virtual void MoveTo(FVector WorldMoveToLocation);
	
	
	
};
