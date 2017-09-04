// Fill out your copyright notice in the Description page of Project Settings.

#include "GalacticEngagementGameModeBase.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Interfaces/GEPlanetInterface.h"
#include "GEBaseShip.h"
#include "Controllers/GEMeleePlayerController.h"
AGalacticEngagementGameModeBase::AGalacticEngagementGameModeBase()
{
	DefaultPawnClass = AGEBaseShip::StaticClass();
	PlayerControllerClass = AGEMeleePlayerController::StaticClass();
}

void AGalacticEngagementGameModeBase::BeginPlay()
{
	SpawnAllAstroids();
	SpawnPlanet();
}

void AGalacticEngagementGameModeBase::SpawnAllAstroids()
{
	if (AstroidClass)
	{
		for (int32 i = 0; i < NumberOfAstroids; i++)
		{
			FTransform transform;

			float X = FMath::RandRange(-AstroidSpawnRadius, AstroidSpawnRadius);
			float Y = FMath::RandRange(-AstroidSpawnRadius, AstroidSpawnRadius);

			transform.SetLocation(FVector(X, Y, AstroidZPlane));
			GetWorld()->SpawnActor(AstroidClass, &transform);
		}
	}
}

void AGalacticEngagementGameModeBase::SpawnPlanet()
{
	if (PlanetClass)
	{
		FTransform transform;
		transform.SetLocation(FVector(0, 0, PlanetZPlane));
		AActor* actor = GetWorld()->SpawnActor(PlanetClass, &transform);
		IGEPlanetInterface::Execute_SetRadius(actor, FMath::RandRange(PlanetMinRadius, PlanetMaxRadius));
	}
}