// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GEBaseShip.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESelectedGun : uint8
{
	SG_Main 			UMETA(DisplayName = "Main Gun"),
	SG_Secondary 	UMETA(DisplayName = "Secondary Gun")
};

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
	class UGEGunBaseComponent* MainGun;
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UGEGunBaseComponent* SecondaryGun;
	
	// debug components
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* FrontFacingArrow;

	FVector2D MoveToPoint;
	float CurrentSpeed;
	float CurrentRotationRate;
	bool FireGunToggle;

public:
	// Sets default values for this pawn's properties
	AGEBaseShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void UpdateMovementRates(float DeltaTime);

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
	UPROPERTY(Category = "Controls",EditAnywhere, BlueprintReadWrite)
	ESelectedGun SelectedGun;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Input
	void MoveTo(float axis);
	
	void FireGunDownMapping();
	void FireGunReleaseMapping();
	virtual void FireSelectedGun();
	/** Called when Weapon Fired*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Control")
	void GunFired(class UGEGunBaseComponent* FiredGun,ESelectedGun TheSelectedGun);
};