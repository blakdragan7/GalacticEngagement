// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/BackgroundPlane.h"
#include "Misc/CustomShipSave.h"
#include "GEMelleePlayerControllerBase.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "GEMeleePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEMeleePlayerController : public AGEMelleePlayerControllerBase
{
	GENERATED_BODY()
private:
	TArray<FBackgroundPlane*> BackgroundPlanes;
	bool BeganStarField;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Environment)
	TArray<TSubclassOf<AActor>> BackgroundClasses;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	TArray<int32> BackgroundLayerDensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	TArray<float> LayerZPlanes;
	
protected:
	virtual void UpdatePlanes();

public:
	AGEMeleePlayerController();
	virtual void Tick(float DeltaTime) override;
	void BeginPlay()override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginDestroy()override;

	UFUNCTION(BlueprintCallable, Category = Background, Client, Reliable)
	void ClientBeginStarField();

	UFUNCTION()
	void LoadCustomShipData();

	UFUNCTION(Server, Reliable, WithValidation)
	void RequestSpawnShip(struct FNetComponentSaveStruct ShipSave);
};
