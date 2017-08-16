// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GEAmmoBase.generated.h"
/* Base Class for all projectile / attacks used by ships */
UCLASS()
class GALACTICENGAGEMENT_API AGEAmmoBase : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* AmmoMesh;

	UPROPERTY(Category = Movement, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	// debug components
	UPROPERTY(Category = Ship, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* FrontFacingArrow;
public:	
	// Sets default values for this actor's properties
	AGEAmmoBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Launch(FVector Velocity);
	
};
