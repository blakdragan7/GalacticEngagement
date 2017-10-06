// Fill out your copyright notice in the Description page of Project Settings.

#include "GEMelleeMatchMakeingGameMode.h"
#include "GEMelleeMatchMakingState.h"
#include "Controllers/GEMatchMakingController.h"

AGEMelleeMatchMakeingGameMode::AGEMelleeMatchMakeingGameMode()
{
	GameStateClass = AGEMelleeMatchMakingState::StaticClass();
	PlayerControllerClass = AGEMatchMakingController::StaticClass();
}

void AGEMelleeMatchMakeingGameMode::BeginPlay()
{
	
}

void AGEMelleeMatchMakeingGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
