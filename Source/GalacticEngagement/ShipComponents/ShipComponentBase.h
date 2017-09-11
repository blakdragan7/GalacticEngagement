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
private:
	class UStaticMesh* ComponentModel;
	EShipComponentType ComponentType;

protected:
	UPROPERTY(Category = "Ship Movement", VisibleAnywhere, BlueprintReadOnly)
	class AGEBaseShip *controlledShip;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType) {};

	FORCEINLINE EShipComponentType GetType() { return ComponentType; }
	FORCEINLINE UStaticMesh* GetModel() { return ComponentModel; }

	FORCEINLINE void SetModel(UStaticMesh* model) { ComponentModel = model; }
};
