// Fill out your copyright notice in the Description page of Project Settings.

#include "GEMelleGameModeBase.h"
#include "Engine/World.h"
#include "GEGameInstance.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Controllers/GEMelleePlayerControllerBase.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"


AGEMelleGameModeBase::AGEMelleGameModeBase()
{
	OnPlayerLostWidget = NULL;
	OnPlayerWonWidget = NULL;

	DelayAfterWinTillTravel = 4.0;
	NumPlayers = 2;
}

void AGEMelleGameModeBase::PlayerControllerDestroyedController(AGEMelleePlayerControllerBase* destroyer, AGEMelleePlayerControllerBase* destroyed)
{
	if (OnPlayerWonWidget && destroyer)destroyer->Client_ShowWidget(OnPlayerWonWidget, DelayAfterWinTillTravel);
	if (OnPlayerLostWidget && destroyed)destroyed->Client_ShowWidget(OnPlayerLostWidget, DelayAfterWinTillTravel);

	controllersToEndSession.Add(destroyer);
	controllersToEndSession.Add(destroyed);

	GetWorld()->GetTimerManager().SetTimer(WinTimer,this,&AGEMelleGameModeBase::CloseSession, DelayAfterWinTillTravel,false);
}

void AGEMelleGameModeBase::CloseSession()
{
	if (UGEGameInstance* instance = Cast<UGEGameInstance>(GetGameInstance()))
	{
		instance->EndMatch();

		for (auto controller : controllersToEndSession)
		{
			controller->Client_EndSession();
		}
	}
}