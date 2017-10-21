// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "GameFramework/PlayerController.h"
#include "GEMelleePlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEMelleePlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
private:
	class UUserWidget* showedWidget;

	FTimerHandle timerHandle;
public:

	AGEMelleePlayerControllerBase();

	UFUNCTION(Client,Reliable)
	void Client_ShowWidget(TSubclassOf<class UUserWidget> widgetToShow,float ForTime=-1);
	
	UFUNCTION(Client, Reliable)
	void Client_EndSession();

	UFUNCTION()
	void RemoveShownWidget();
};
