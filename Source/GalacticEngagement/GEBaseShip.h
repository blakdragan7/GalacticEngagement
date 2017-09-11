// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/GEDamageInterface.h"
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
	UPROPERTY(Category = UI, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* ShipWidget;
	// debug components
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* FrontFacingArrow;

	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CameraOffsetScale;

	bool FireGunToggle;
	TArray<AGEBaseShip*> AllCurrentlyTargetingMe;
	bool HasMovementInput;

	class GETravelManagerBase* TravelManager;

protected:
	
	UPROPERTY(Category = "Ship AI", EditAnywhere, BlueprintReadWrite)
	AGEBaseShip* CurrentlyTargetedShip;

	UPROPERTY(Category = "Ship AI", EditAnywhere, BlueprintReadWrite)
	float AttackDistance;

	UPROPERTY(Category = "Ship AI", EditAnywhere, BlueprintReadWrite)
	float AttackAngle; 


private:
	void UpdateCameraPosition();
	void UpdateInputs();

public:
	// Sets default values for this pawn's properties
	AGEBaseShip();
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(Category = "Ship Stats", VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentHealth;
	UPROPERTY(Category = "Ship Stats", VisibleAnywhere, BlueprintReadOnly)
	int32 MaxHealth;

	UPROPERTY(Category = "Ship UI", EditAnywhere, BlueprintReadWrite)
	int32 StartingCameraArmLength;

	UPROPERTY(Category = "Controls",EditAnywhere, BlueprintReadWrite)
	ESelectedGun SelectedGun;
	UPROPERTY(Category = UI, EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> ShipHUDWidget;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/** Damage Interface */
	virtual void ReceiveDamage(int32 Damage, FVector DamageLocation) override;
	virtual int32 GetHealth() override;
	/** End Damage Interface*/
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Input
	void MoveToDown();
	void MoveToUp();
	
	FVector GetCurrentForwardVector();
	FRotator GetCurrentRotation();
	FVector GetCurrentLocation();
	void SetRotation(FQuat rotation);
	void SetRotation(FRotator rotation);

	void FireGunDownMapping();
	void FireGunReleaseMapping();
	virtual bool SetLocation(const FVector& NewLocation, bool bSweep, FHitResult& OutSweepHitResult); // Work around for root component being a scene component
	virtual void FireSelectedGun();
	virtual void OnShipDeath();
	virtual void WasTargetBy(AGEBaseShip* aggresser);

	/*
	* Add a Velcoity Effector to this ship
	* An Effector is an object that affects the veleocity of this ship
	* somehow exlcuding things like bouncing, so a gravity well like a planet
	* would count but an astroid that would only effect this ship by bouncing
	* after collision would not cound
	*/
	UFUNCTION(BlueprintCallable, Category = "Control")
	void AddVelocityEffector(AActor* effector);
	UFUNCTION(BlueprintCallable, Category = "Control")
	void removeVelocityEffector(AActor* effector);
	/*
	* Called Every Frame, percentage is the current percent the thruster is runnin compared to max or (currentspeed / maxspeed)
	* This is meant for doing effects such as showing a flame behind the ship etc.. to show that the ship is moving
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void Thrusting(float percentage);
	/** Called when Weapon Fired*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Control")
	void GunFired(class UGEGunBaseComponent* FiredGun,ESelectedGun TheSelectedGun);
	/** Called When Ship is About to be Destroyed. 
	**	After this function returns the Actor will be destroy so 
	**	starting timers or anything with a delay here wouild not work */
	UFUNCTION(BlueprintImplementableEvent, Category = "Control")
	void ShipDestroyed();

	UFUNCTION(BlueprintPure, Category = "Ship Stats")
	float GetHealthPercentage();
	/** AI Functions */
	void MoveTo(AActor* Actor);
	virtual void InvalidateTarget();
	virtual bool ShouldFireGun();
	virtual bool ShouldFireMainGun();
	virtual bool ShouldFireSecondaryGun();
	virtual void SearchForTarget(float radius);
	virtual bool CanBeTargetedBy(AGEBaseShip* other);
	virtual AGEBaseShip* GetTarget();
};