// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GEDamageInterface.h"
#include "GEBaseShip.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ESelectedGun : uint8
{
	SG_Main 			UMETA(DisplayName = "Main Gun"),
	SG_Secondary 	UMETA(DisplayName = "Secondary Gun")
};

UCLASS()
class GALACTICENGAGEMENT_API AGEBaseShip : public APawn, public IGEDamageInterface
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

	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CameraOffsetScale;

	FVector2D MoveToPoint;
	float CurrentSpeed;
	float CurrentRotationRate;
	bool FireGunToggle;
	AGEBaseShip* CurrentlyTargetingMe;

protected:
	
	UPROPERTY(Category = "Ship AI", EditAnywhere, BlueprintReadWrite)
	AGEBaseShip* CurrentlyTargetedShip;

	UPROPERTY(Category = "Ship AI", EditAnywhere, BlueprintReadWrite)
	float AttackDistance;

	UPROPERTY(Category = "Ship AI", EditAnywhere, BlueprintReadWrite)
	float AttackAngle; 

	UPROPERTY(Category = "Ship AI", EditAnywhere, BlueprintReadWrite)
	float SlowDownDistance;

private:
	void UpdateCameraPosition();

protected:
	// Called when the game starts or when spawned
	virtual void UpdateMovementRates(float DeltaTime);

public:
	// Sets default values for this pawn's properties
	AGEBaseShip();
	virtual void BeginPlay() override;

public:
	UPROPERTY(Category = "Ship Mechanics", VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentHealth;
	UPROPERTY(Category = "Ship Mechanics", VisibleAnywhere, BlueprintReadOnly)
	int32 MaxHealth;
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
	/** Damage Interface */
	virtual void ReceiveDamage(int32 Damage, FVector DamageLocation) override;
	virtual int32 GetHealth() override;
	/** End Damage Interface*/
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Input
	void MoveTo(float axis);
	
	void FireGunDownMapping();
	void FireGunReleaseMapping();
	virtual void FireSelectedGun();
	virtual void OnShipDeath();
	virtual void WasTargetBy(AGEBaseShip* aggresser);
	/** Called when Weapon Fired*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Control")
	void GunFired(class UGEGunBaseComponent* FiredGun,ESelectedGun TheSelectedGun);
	/** Called When Ship is About to be Destroyed. 
	**	After this function returns the Actor will be destroy so 
	**	starting timers or anything with a delay here wouild not work */
	UFUNCTION(BlueprintImplementableEvent, Category = "Control")
	void ShipDestroyed();

	/** AI Functions */
	void MoveTo(AActor* Actor);
	virtual bool ShouldFireGun();
	virtual bool ShouldFireMainGun();
	virtual bool ShouldFireSecondaryGun();
	virtual void SearchForTarget(float radius);
	virtual bool CanBeTargetedBy(AGEBaseShip* other);
	virtual AGEBaseShip* GetTarget();
};