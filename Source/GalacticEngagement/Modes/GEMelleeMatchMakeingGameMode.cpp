// Fill out your copyright notice in the Description page of Project Settings.

#include "GEMelleeMatchMakeingGameMode.h"
#include "Engine.h"
#include "GEMelleeMatchMakingState.h"
#include "Controllers/GEMatchMakingController.h"

AGEMelleeMatchMakeingGameMode::AGEMelleeMatchMakeingGameMode()
{
	GameStateClass = AGEMelleeMatchMakingState::StaticClass();
	PlayerControllerClass = AGEMatchMakingController::StaticClass();
	bUseSeamlessTravel = true;
}

void AGEMelleeMatchMakeingGameMode::BeginPlay()
{
	
}

void AGEMelleeMatchMakeingGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AllControllers.AddUnique(NewPlayer);
	if (AGEMatchMakingController* mmController = Cast<AGEMatchMakingController>(NewPlayer))
	{
		mmController->MultiNewPlayerLogin();
	}
}

void AGEMelleeMatchMakeingGameMode::Logout(AController * controller)
{
	Super::Logout(controller);
	AllControllers.Remove(controller);
}
