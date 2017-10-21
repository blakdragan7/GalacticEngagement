// Fill out your copyright notice in the Description page of Project Settings.

#include "GEMelleePlayerControllerBase.h"
#include "GEGameInstance.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

AGEMelleePlayerControllerBase::AGEMelleePlayerControllerBase()
{
	showedWidget = 0;
}

void AGEMelleePlayerControllerBase::Client_ShowWidget_Implementation(TSubclassOf<class UUserWidget> widgetToShow, float ForTime)
{
	if (IsLocalPlayerController() == false)
	{
		UE_LOG(LogTemp,Error,TEXT("Can not run AGEMelleePlayerControllerBase::ClientShowWidget on remote "));
		return;
	}
	this->RemoveShownWidget();
	showedWidget = CreateWidget<UUserWidget>(GetWorld(), widgetToShow);
	showedWidget->AddToViewport();

	if (ForTime > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &AGEMelleePlayerControllerBase::RemoveShownWidget, ForTime, false);
	}
}

void AGEMelleePlayerControllerBase::Client_EndSession_Implementation()
{
	if (Role != ROLE_Authority)
	{
		if (UGEGameInstance* instance = Cast<UGEGameInstance>(GetGameInstance()))
		{
			instance->EndMatch();
		}
	}
}

void AGEMelleePlayerControllerBase::RemoveShownWidget()
{
	if (IsValid(showedWidget))
	{
		showedWidget->RemoveFromParent();
		showedWidget = 0;
	}
}
