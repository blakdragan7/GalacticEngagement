// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GEBaseShip.generated.h"

UCLASS()
class GALACTICENGAGEMENT_API AGEBaseShip : public APawn
{
	GENERATED_BODY()
private:
	// components
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent * CameraBoom;
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipBody;
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UGEEngineBaseComponent* Engine;
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UGEThrusterBaseComponent * Thrusters;
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UGEGunBaseComponent* Guns;
	
	// debug components
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* FrontFacingArrow;

	FVector2D MoveToPoint;
	float CurrentSpeed;
	float CurrentRotationRate;

public:
	// Sets default values for this pawn's properties
	AGEBaseShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(Category = "Ship Movement", VisibleAnywhere, BlueprintReadOnly)
	bool IsMoving;
	UPROPERTY(Category = "Ship Movement", EditAnywhere, BlueprintReadWrite)
	float GroundZ;
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void UpdateMovementRates(float DeltaTime);
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Input
	void MoveTo(float axis);
};