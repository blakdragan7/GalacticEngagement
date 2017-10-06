// Fill out your copyright notice in the Description page of Project Settings.

#include "GEMatchMakingController.h"
#include "Modes/GEMelleeMatchMakeingGameMode.h"
#include "Runtime/Engine/Classes/Engine/LocalPlayer.h"
#include "Engine/World.h"

AGEMatchMakingController::AGEMatchMakingController()
{
	bShowMouseCursor = true;
}

void AGEMatchMakingController::BeginPlay()
{
	if (Role != ROLE_Authority)
	{
		
	}
}
