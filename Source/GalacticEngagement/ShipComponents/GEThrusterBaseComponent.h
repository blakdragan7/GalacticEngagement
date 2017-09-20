// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShipComponentBase.h"
#include "Components/StaticMeshComponent.h"
#include "GEThrusterBaseComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GALACTICENGAGEMENT_API UGEThrusterBaseComponent : public UShipComponentBase
{
	GENERATED_BODY()
private:
	FVector2D ScreenMoveToLocation;
	FVector WorldMoveToLocation;
	FVector CurrentVelocity;
	FVector ExternalForce;
	FRotator DesiredRotation;
	float CurrentRotationRate;
	bool NeedsScreenDirectionUpdate;
	bool NeedsWorldDirectionUpdate;
	bool HitLastFrame;

	TArray<AActor*> Effectors;

	void Thrusting(float percentage);

protected:
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

	virtual void TickComponent(float DeltaTime)override;

	virtual void UpdateVelocityFromEffectors(FVector CurrentPosition, float DeltaTime);
	virtual void UpdateMovementRates(FVector Direction, float DeltaTime);
	virtual void StopMoving();
	virtual void MoveTo(FVector2D ScrenMoveToLocation);
	virtual void MoveTo(FVector WorldMoveToLocation);
	virtual bool AddEffector(AActor * actor);
	virtual bool RemoveEffector(AActor * actor);
};
