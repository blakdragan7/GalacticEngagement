// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GalacticEngagementGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGalacticEngagementGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	TArray<AActor*> Stars;
	TArray<AActor*> Astroids;
	AActor*			Planet;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Environment)
	TSubclassOf<AActor> AstroidClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Environment)
	TSubclassOf<AActor> PlanetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	int32 NumberOfAstroids;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	float AstroidSpawnRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	float PlanetMaxRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	float PlanetMinRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	float PlanetZPlane;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	float AstroidZPlane;
	

private:
	void SpawnAllAstroids();
	void SpawnPlanet();

public:
	AGalacticEngagementGameModeBase();
	void BeginPlay()override;
};
