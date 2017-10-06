// Fill out your copyright notice in the Description page of Project Settings.

#include "GEGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/Engine/LocalPlayer.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

UGEGameInstance::UGEGameInstance()
{
	/** Bind function for CREATING a Session */
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UGEGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UGEGameInstance::OnStartOnlineGameComplete);

	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UGEGameInstance::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UGEGameInstance::OnJoinSessionComplete);

	bHasSession = false;
}

void UGEGameInstance::CreateSession()
{
	if (bHasSession)
	{
		UE_LOG(LogTemp,Warning,TEXT("Already Has Sessions !!"));
		return;
	}
	// Get the Online Subsystem to work with
	OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface, so we can call the "CreateSession" function on it
		Sessions = OnlineSub->GetSessionInterface();

		FOnlineSessionSettings SessionSettings;

		SessionSettings.bIsLANMatch = true;
		SessionSettings.bUsesPresence = false;
		SessionSettings.NumPublicConnections = 100;
		SessionSettings.NumPrivateConnections = 0;
		SessionSettings.bAllowInvites = true;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;

		SessionSettings.Set(SETTING_MAPNAME, FString("MatchMakingMap"), EOnlineDataAdvertisementType::ViaOnlineService);

		// Set the delegate to the Handle of the SessionInterface
		OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

		// Our delegate should get called when this is complete (doesn't need to be successful!)
		bool success = Sessions->CreateSession(*GetFirstGamePlayer()->GetPreferredUniqueNetId(), FName("MatchMakingSession"), SessionSettings);
		if (success == false)UE_LOG(LogTemp, Warning, TEXT("Failed to create Session !!"))
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
	}
}

void UGEGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
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

void UGEGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the Online Subsystem so we can get the Session Interface
	if (OnlineSub)
	{
		// Get the Session Interface to clear the Delegate
		Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the delegate, since we are done with this call
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}

	}

	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "MatchMakingMap", true, "listen");
		bHasSession = true;
	}
}

void UGEGameInstance::StartFindSession()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FindSessions(Player->GetPreferredUniqueNetId(), true, false);
}

void UGEGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Looking For Session UserId %s bIsLAN %i bIsPresence %i"), *UserId->ToString(), (int)bIsLAN, (int)bIsPresence));
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			/*
			Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
			*/
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 20000;
			SessionSearch->PingBucketSize = 50;

			// We only want to set this Query Setting if "bIsPresence" is true
			if (bIsPresence)
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}

			SessionSearch->QuerySettings.Set(SETTING_MAPNAME, FString("MatchMakingMap"), EOnlineComparisonOp::Equals);
			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			// Set the Delegate to the Delegate Handle of the FindSession function
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	}
	else
	{
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}
}

bool UGEGameInstance::JoinSession(ULocalPlayer * LocalPlayer, const FOnlineSessionSearchResult & SearchResult)
{
	// Return bool
	bool bSuccessful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && IsValid(LocalPlayer))
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Sessions->JoinSession(LocalPlayer->GetUniqueID(), "MatchMakingSession", SearchResult);
		}
	}

	return bSuccessful;
}

void UGEGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OFindSessionsComplete bSuccess: %d"), bWasSuccessful));

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the Delegate handle, since we finished this call
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			// Just debugging the Number of Search results. Can be displayed in UMG or something later on
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));

			// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
			if (SessionSearch->SearchResults.Num() > 0)
			{
				// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
				// This can be customized later on with your own classes to add more information that can be set and displayed
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{
					// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
					// This is something you can't do in Blueprint for example!
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session Number: %d | Sessionname: %s "), SearchIdx + 1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName)));
				}
			}
		}
	}
}

void UGEGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = IOnlineSubsystem::Get()->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the Delegate again
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
			// This is something the Blueprint Node "Join Session" does automatically!
			APlayerController * const PlayerController = GetFirstLocalPlayerController();


			// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
			// String for us by giving him the SessionName and an empty String. We want to do this, because
			// Every OnlineSubsystem uses different TravelURLs
			FString TravelURL;

			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				// Finally call the ClienTravel. If you want, you could print the TravelURL to see
				// how it really looks like
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}