// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GEMelleeMatchMakingState.generated.h"

/**
 * 
 */
UCLASS()
class GALACTICENGAGEMENT_API AGEMelleeMatchMakingState : public AGameStateBase
{
	GENERATED_BODY()
private:


public:
	AGEMelleeMatchMakingState();
	
	/** Add PlayerState to the PlayerArray */
	virtual void AddPlayerState(APlayerState* PlayerState)override;

	/** Remove PlayerState from the PlayerArray. */
	virtual void RemovePlayerState(APlayerState* PlayerState)override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Network Update")
	void PlayerJoinedMatchMakingSession(const FString &Name);

	UFUNCTION(BlueprintImplementableEvent, Category = "Network Update")
	void PlayerLeftMatchMakingSession(const FString &Name);
};
