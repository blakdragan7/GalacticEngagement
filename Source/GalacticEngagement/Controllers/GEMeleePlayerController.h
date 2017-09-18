// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/BackgroundPlane.h"
#include "GameFramework/PlayerController.h"
#include "GEMeleePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEMeleePlayerController : public APlayerController
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
	virtual void BeginDestroy();

	UFUNCTION(BlueprintCallable,Category=Background)
	void BeginStarField();
};
