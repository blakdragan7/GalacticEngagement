// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "ComponentMountPoint.generated.h"

UENUM(BlueprintType)
enum class EComponentMountType : uint8
{
	CM_Engine 			UMETA(DisplayName = "Engine"),
	CM_Thruster 		UMETA(DisplayName = "Thruster"),
	CM_MainGun 			UMETA(DisplayName = "Main Gun"),
	CM_SecondaryGun 	UMETA(DisplayName = "Secondary Gun")
};

UCLASS( ClassGroup=(ShipComponents), meta=(BlueprintSpawnableComponent) )
class GALACTICENGAGEMENT_API UComponentMountPoint : public UArrowComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(Category = Debug, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* FrontFacingArrow;

public:	
	// Sets default values for this component's properties
	UComponentMountPoint();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Category = Type, EditAnywhere, BlueprintReadWrite)
	EComponentMountType type;
};
