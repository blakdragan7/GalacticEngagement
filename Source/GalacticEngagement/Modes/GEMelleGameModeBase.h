// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GalacticEngagementGameModeBase.h"
#include "Runtime/CoreUObject/Public/Templates/SubclassOf.h"
#include "GEMelleGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEMelleGameModeBase : public AGalacticEngagementGameModeBase
{
	GENERATED_BODY()
private:
	UPROPERTY(Category = UI, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> OnPlayerLostWidget;
	UPROPERTY(Category = UI, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> OnPlayerWonWidget;

	TArray<class AGEMelleePlayerControllerBase*> controllersToEndSession;

	UPROPERTY(Category = UI, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float DelayAfterWinTillTravel;

	UPROPERTY(Category = UI, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 NumPlayers;

	FTimerHandle WinTimer;
public:
	AGEMelleGameModeBase();

	UFUNCTION()
	void PlayerControllerDestroyedController(class AGEMelleePlayerControllerBase* destroyer, class AGEMelleePlayerControllerBase* destroyed);
	
	void CloseSession();
};
