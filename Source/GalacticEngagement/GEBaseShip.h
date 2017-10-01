// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GEGestureHandler.h"
#include "GameFramework/Pawn.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "ShipComponents/ShipComponentTypes.h"
#include "Interfaces/GEDamageInterface.h"
#include "GEBaseShip.generated.h"

UENUM(Blueprintable, BlueprintType)		//"BlueprintType" is essential to include
enum class ESelectedGun : uint8
{
	SG_Main 			UMETA(DisplayName = "Main Gun"),
	SG_Secondary 	UMETA(DisplayName = "Secondary Gun")
};

UCLASS()
class GALACTICENGAGEMENT_API AGEBaseShip : public APawn, public IGEDamageInterface, public IGEGestureHandlerDelegate
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

	float CurrentCameraDistance;

	bool HasCameraDistance;

	class GETravelManagerBase* TravelManager;

	UPROPERTY()
	UGEGestureHandler* GestureHandler;

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

	void SetToMaxCaemraDistance(); // Exposed for controller to get max distance
	void ResetToNormalCameraDistance(); // Resets cmaera from max distance, if called without a previous call to SetToMaxCaemraDistance sets the camera to smallest distance

public:
	UPROPERTY(Category = "Ship Stats", VisibleAnywhere, BlueprintReadOnly, Replicated)
	int32 CurrentHealth;
	UPROPERTY(Category = "Ship Stats", VisibleAnywhere, BlueprintReadOnly)
	int32 MaxHealth;

	UPROPERTY(Category = "Ship UI", EditAnywhere, BlueprintReadWrite)
	int32 MaxCameraArmLength;
	UPROPERTY(Category = "Ship UI", EditAnywhere, BlueprintReadWrite)
	int32 StartingCameraArmLength;

	UPROPERTY(Category = "Controls",EditAnywhere, BlueprintReadWrite)
	ESelectedGun SelectedGun;
	UPROPERTY(Category = UI, EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> ShipHUDWidget;

	UPROPERTY(Category = Mounts, EditAnywhere, BlueprintReadWrite)
	class UComponentMountPoint* EngineMount;
	UPROPERTY(Category = Mounts, EditAnywhere, BlueprintReadWrite)
	class UComponentMountPoint* ThrusterMount;

	UPROPERTY(Category = Mounts, EditAnywhere, BlueprintReadWrite)
	TArray<class UComponentMountPoint*> MainGunComponents;
	UPROPERTY(Category = Mounts, EditAnywhere, BlueprintReadWrite)
	TArray<class UComponentMountPoint*> SecondaryGunComponents;

	/* Network Functions */

	UFUNCTION(Server, WithValidation, Reliable)
	void Server_ReceiveDamage(int32 Damage, FVector DamageLocation);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_ReceiveDamage(int32 Damage, FVector DamageLocation);

	UFUNCTION(Server, WithValidation, Reliable)
	virtual void Server_SetLocation(const FVector& NewLocation); 

	UFUNCTION(NetMulticast, Reliable)
	virtual void MultiCast_SetLocation(const FVector& NewLocation);

	UFUNCTION(Server, WithValidation, Reliable)
	void Server_SetRotation(FRotator rotation);
	UFUNCTION(NetMulticast, Reliable)
	void MultiCast_SetRotation(FRotator rotation);


	UFUNCTION(Server, WithValidation, Reliable)
	void Server_UpdateComponentsWithInput(FVector2D ScreenPosition);

	UFUNCTION(Server, WithValidation, Reliable)
	void Server_FireSelectedGun();

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

	/* Returns Closest Mount To Screen Point within range Or Null if none */
	UFUNCTION(BlueprintCallable, Category = "Control")
	class UComponentMountPoint* ClosestMountToPoint(FVector2D screen_point,float range);

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
	UFUNCTION(BlueprintCallable, Category = "Ship Config")
	void AssignComponent(TSubclassOf<class UShipComponentBase> component,EShipComponentType type,FVector2D inScreenPosition);
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


	/* PGEGestureHandlerDelegate Implentation */
	virtual void DoubleTap(float x, float y) override;
	virtual void SingleTap(float x, float y) override;
	virtual void ConstTapStart(float x, float y) override;
	virtual void ConstTapEnd(float x, float y) override;
	virtual void SwipeStart(float x, float y) override;
	virtual void SwipeUpdate(float x, float y) override;
	virtual void SwipeEnd(float x, float y) override;
};
