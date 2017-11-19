// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShipComponentTypes.h"
#include "UObject/NoExportTypes.h"
#include "ShipComponentBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GALACTICENGAGEMENT_API UShipComponentBase : public UObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(Category = "Ship Movement", VisibleAnywhere, BlueprintReadOnly)
	class AGEBaseShip *ControlledShip;
	class UStaticMesh* ComponentModel;
	EShipComponentType ComponentType;
	class UComponentMountPoint* MountedLocation;

public:
	virtual void TickComponent(float DeltaTime) {};

	FORCEINLINE EShipComponentType GetType() { return ComponentType; }
	FORCEINLINE UStaticMesh* GetModel() { return ComponentModel; }

	FORCEINLINE void SetModel(UStaticMesh* model) { ComponentModel = model; }

	bool AssignToMountPoint(class UComponentMountPoint* MountePoint);

	bool IsSupportedForNetworking() const override
	{
		return true;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Statistics")
	FString Description;
};
