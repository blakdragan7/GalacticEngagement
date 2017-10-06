// Fill out your copyright notice in the Description page of Project Settings.

#include "GEMelleeMatchMakingState.h"
#include "GameFramework/PlayerState.h"

AGEMelleeMatchMakingState::AGEMelleeMatchMakingState()
{

}

void AGEMelleeMatchMakingState::AddPlayerState(APlayerState* PlayerState)
{
	PlayerJoinedMatchMakingSession(PlayerState->PlayerName);
}

void AGEMelleeMatchMakingState::RemovePlayerState(APlayerState* PlayerState)
{
	PlayerLeftMatchMakingSession(PlayerState->PlayerName);
}