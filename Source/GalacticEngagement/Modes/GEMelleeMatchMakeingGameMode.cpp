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
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player Logged In ! %i"), NewPlayer->GetLocalPlayer()->GetUniqueID()));
}
