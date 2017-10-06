// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Online.h"
#include "GEMelleeMatchMakeingGameMode.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GALACTICENGAGEMENT_API AGEMelleeMatchMakeingGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	IOnlineSubsystem* OnlineSub;
	IOnlineSessionPtr Sessions;

public:
	AGEMelleeMatchMakeingGameMode();

	virtual void BeginPlay()override;
	virtual void PostLogin(APlayerController* NewPlayer)override;
	
	bool TryConnectPlayers(int32 PlayerID, int32 PlayerID2);

	UFUNCTION(BlueprintCallable)
	void CreateSession();

	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
};
