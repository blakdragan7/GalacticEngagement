// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GEMelleeMatchMakeingGameMode.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GALACTICENGAGEMENT_API AGEMelleeMatchMakeingGameMode : public AGameModeBase
{
	GENERATED_BODY()
private:
	UPROPERTY(Category = Session, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AController*> AllControllers;

public:
	AGEMelleeMatchMakeingGameMode();

	virtual void BeginPlay()override;
	virtual void PostLogin(APlayerController* NewPlayer)override;
	virtual void Logout(AController* controller) override;
};
