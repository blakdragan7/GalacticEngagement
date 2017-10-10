// Fill out your copyright notice in the Description page of Project Settings.

#include "GEPlayerMelleeGameMode.h"
#include "Controllers/GEMeleePlayerController.h"
#include "Misc/CustomShipSave.h"
#include "Math/GEGameStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GEBaseShip.h"

AGEPlayerMelleeGameMode::AGEPlayerMelleeGameMode()
{
	PlayerControllerClass = AGEMeleePlayerController::StaticClass();
}

void AGEPlayerMelleeGameMode::HandleStartingNewPlayer_Implementation(APlayerController * NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	AllControllers.AddUnique(NewPlayer);
}

void AGEPlayerMelleeGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AllControllers.AddUnique(NewPlayer);
}

void AGEPlayerMelleeGameMode::Logout(AController * controller)
{
	Super::Logout(controller);
	AllControllers.Remove(controller);
}

void AGEPlayerMelleeGameMode::SpawnPlayer(AController* requestnigController, struct FNetComponentSaveStruct& shipSave)
{
	if (APawn* pawn = requestnigController->GetPawn())
	{
		pawn->Destroy();
	}

	FActorSpawnParameters params;

	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform transform;

	transform.SetLocation(FVector(FMath::RandRange(-3000,3000), FMath::RandRange(-3000, 3000),210));

	if (AGEBaseShip* ship = GetWorld()->SpawnActor<AGEBaseShip>(shipSave.ShipClass, transform, params))
	{
		GEGameStatics::PopulateShipFromNetStruct(ship, shipSave);

		requestnigController->Possess(ship);
		if (AGEMeleePlayerController* MPC = Cast<AGEMeleePlayerController>(requestnigController))
		{
			MPC->ClientBeginStarField();
		}
	}
}
