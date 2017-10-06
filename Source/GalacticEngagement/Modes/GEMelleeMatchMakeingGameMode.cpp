// Fill out your copyright notice in the Description page of Project Settings.

#include "GEMelleeMatchMakeingGameMode.h"
#include "GEMelleeMatchMakingState.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Controllers/GEMatchMakingController.h"

AGEMelleeMatchMakeingGameMode::AGEMelleeMatchMakeingGameMode()
{
	GameStateClass = AGEMelleeMatchMakingState::StaticClass();
	PlayerControllerClass = AGEMatchMakingController::StaticClass();

	/** Bind function for CREATING a Session */
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &AGEMelleeMatchMakeingGameMode::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &AGEMelleeMatchMakeingGameMode::OnStartOnlineGameComplete);

}

void AGEMelleeMatchMakeingGameMode::BeginPlay()
{
	
}

void AGEMelleeMatchMakeingGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

bool AGEMelleeMatchMakeingGameMode::TryConnectPlayers(int32 PlayerID, int32 PlayerID2)
{
	return false;
}

void AGEMelleeMatchMakeingGameMode::CreateSession()
{
	// Get the Online Subsystem to work with
	OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface, so we can call the "CreateSession" function on it
		Sessions = OnlineSub->GetSessionInterface();

		FOnlineSessionSettings SessionSettings;

		SessionSettings.bIsLANMatch = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.NumPublicConnections = 4;
		SessionSettings.NumPrivateConnections = 0;
		SessionSettings.bAllowInvites = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;

		SessionSettings.Set(SETTING_MAPNAME, FString("MatchMakingMap"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		// Set the delegate to the Handle of the SessionInterface
		OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

		// Our delegate should get called when this is complete (doesn't need to be successful!)
		bool success = Sessions->CreateSession(10, FName("MatchMakingSession"), SessionSettings);
		if (success == false)UE_LOG(LogTemp, Warning, TEXT("Failed to create Session !!"))
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
	}
}

void AGEMelleeMatchMakeingGameMode::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem so we can get the Session Interface
	if (OnlineSub)
	{
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the SessionComplete delegate handle, since we finished this call
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				// Our StartSessionComplete delegate should get called after this
				Sessions->StartSession(SessionName);
			}
		}

	}
}

void AGEMelleeMatchMakeingGameMode::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the Online Subsystem so we can get the Session Interface
	if (OnlineSub)
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the delegate, since we are done with this call
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if (bWasSuccessful)
	{
		//UGameplayStatics::OpenLevel(GetWorld(), "MatchMakingSession", true, "listen");
	}
}
