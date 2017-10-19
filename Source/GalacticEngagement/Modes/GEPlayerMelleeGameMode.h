// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modes/GEMelleGameModeBase.h"
#include "GEPlayerMelleeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEPlayerMelleeGameMode : public AGEMelleGameModeBase
{
	GENERATED_BODY()

private:
	UPROPERTY(Category = Session, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AController*> AllControllers;

public:
	AGEPlayerMelleeGameMode();

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)override;

	virtual void PostLogin(APlayerController* NewPlayer)override;
	virtual void Logout(AController* controller) override;
	void SpawnPlayer(AController* requestnigController,struct FNetComponentSaveStruct& shipSave);
};
