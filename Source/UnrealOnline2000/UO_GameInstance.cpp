// Fill out your copyright notice in the Description page of Project Settings.


#include "UO_GameInstance.h"
#include "Online/OnlineSessionNames.h"

void UUO_GameInstance::Init()
{
	Super::Init();

	GEngine->OnNetworkFailure().AddUObject(this, &UUO_GameInstance::NetworkFailureOccurred);

	if (IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get())
	{
		SessionInterface = OnlineSub->GetSessionInterface();

		// Check if the Session Interface is Valid
		if (SessionInterface.IsValid() == true)
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UUO_GameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UUO_GameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UUO_GameInstance::OnJoinSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UUO_GameInstance::OnDestroySessionComplete);
		}
	}
}

void UUO_GameInstance::OnCreateSessionComplete(FName _sessionName, bool _bSuccess)
{
	// Session was Successfully Created
	if (_bSuccess == true)
	{
		FString LevelOptions = FString::Printf(
			TEXT("?listen?ScoreLimit=%d?MaxPlayers=%d"),
			HostSettings.ScoreLimit,
			HostSettings.MaxPlayers
		);

		// LAN Flag
		if (SessionSettings->bIsLANMatch == true) LevelOptions.Append("?bIsLanMatch=1");

		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
			FString::Printf(TEXT("Session created! Travelling with: %s"), *LevelOptions));

		GetWorld()->ServerTravel(TEXT("Lvl_ThirdPerson") + LevelOptions);
	}
}

void UUO_GameInstance::OnFindSessionsComplete(bool _bSuccess)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("FindSessionsComplete bSuccess: %d"), _bSuccess));
	if (SessionInterface.IsValid() == false) return;

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));

	for (int i = 0; i < SessionSearch->SearchResults.Num(); i++)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
			FString::Printf(TEXT("Session Number: %d | Session Name: %s"), i + 1, *SessionSearch->SearchResults[i].Session.OwningUserName));
	}

	OnSessionsFound.Broadcast();
}

void UUO_GameInstance::OnJoinSessionComplete(FName _sessionName, EOnJoinSessionCompleteResult::Type _result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
			FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *_sessionName.ToString(), static_cast<int32>(_result)));
	
	if (_result != EOnJoinSessionCompleteResult::Success)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Join FAILED!"));
		return;
	}

	if (SessionInterface.IsValid() == false) return;

	APlayerController* const PlayerController = GetFirstLocalPlayerController();

	FString TravelURL;

	if (PlayerController && SessionInterface->GetResolvedConnectString(_sessionName, TravelURL))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
			FString::Printf(TEXT("Travelling to: %s"), *TravelURL));

		PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			TEXT("GetResolvedConnectString FAILED, TravelURL is empty"));
	}
}

void UUO_GameInstance::OnDestroySessionComplete(FName _sessionName, bool _bSuccess)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
		FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *_sessionName.ToString(), _bSuccess));
	if (SessionInterface.IsValid() == false) return;

	UGameplayStatics::OpenLevel(GetWorld(), L"MainMenu", true);
}

void UUO_GameInstance::NetworkFailureOccurred(UWorld* _world, UNetDriver* _netDriver, ENetworkFailure::Type _failureType, const FString& _errorString)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
		FString::Printf(TEXT("NetworkFailure: %s"), *_errorString));

	// Destroy if a Session Exists
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession != nullptr)
		{
			DestroySession();
		}
	}
}

bool UUO_GameInstance::HostSession(FKOTHSessionSettings _HostSettings, bool _bIsLAN)
{
	if (SessionInterface.IsValid() == false) return false;

	SessionSettings = MakeShareable(new FOnlineSessionSettings());

	// Set the Session Settings
	SessionSettings->bIsLANMatch = _bIsLAN;
	SessionSettings->bUsesPresence = true;
	SessionSettings->NumPublicConnections = _HostSettings.MaxPlayers;
	SessionSettings->NumPrivateConnections = 0;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	SessionSettings->bUseLobbiesIfAvailable = true;

	// Client will read these Custom Keys from FOnlineSessionSearchResult
	SessionSettings->Set(SEARCH_KEYWORDS, FString("Custom"), EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings->Set(FName("SESSION_DISPLAY_NAME"), _HostSettings.SessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName("SCORE_LIMIT"), _HostSettings.ScoreLimit, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	HostSettings = _HostSettings;

	return SessionInterface->CreateSession(0, NAME_GameSession, *SessionSettings);
}

void UUO_GameInstance::FindSessions(TSharedPtr<const FUniqueNetId> _userID, bool _bIsLAN)
{
	if (SessionInterface.IsValid() == false) { OnFindSessionsComplete(false); return; }
	if (_userID.IsValid() == false) { OnFindSessionsComplete(false); return; }

	SessionSearch = MakeShareable(new FOnlineSessionSearch);

	// Set the Session Search Settings
	SessionSearch->bIsLanQuery = _bIsLAN;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->PingBucketSize = 50;
	
	// Set the Session Search Query Settings (for Steam)
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, FString("Custom"), EOnlineComparisonOp::Equals);

	// Find Sessions
	SessionInterface->FindSessions(*_userID, SessionSearch.ToSharedRef());
}

bool UUO_GameInstance::JoinSession(TSharedPtr<const FUniqueNetId> _userID, FName _sessionName, const FOnlineSessionSearchResult& _searchResult)
{
	if (SessionInterface.IsValid() == false) return false;

	return SessionInterface->JoinSession(*_userID, _sessionName, _searchResult);
}

void UUO_GameInstance::StartGame(FKOTHSessionSettings _HostSettings, bool _bLAN)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	HostSession(_HostSettings, _bLAN);
}

void UUO_GameInstance::FindGames(bool _bLAN)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId(), _bLAN);
}

void UUO_GameInstance::JoinGame(int _iServerIndex)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FOnlineSessionSearchResult SearchResult;
	if (SessionSearch->SearchResults.Num() > _iServerIndex)
	{
		SearchResult = SessionSearch->SearchResults[_iServerIndex];

		JoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, SearchResult);
	}
}

void UUO_GameInstance::DestroySession()
{
	if (SessionInterface.IsValid() == false) return;

	SessionInterface->DestroySession(NAME_GameSession);
}

TArray<FSessionDisplayInfo> UUO_GameInstance::GetSessionDisplayInfos()
{
	TArray<FSessionDisplayInfo> Out;
	if (SessionSearch.IsValid() == false) return Out;

	// Loop through all the found Sessions
	for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
	{
		// Construct the Session Info as an FSessionDisplayInfo Struct
		const FOnlineSessionSearchResult& Result = SessionSearch->SearchResults[i];
		FSessionDisplayInfo Info;
		Info.ResultIndex = i;
		Info.CurrentPlayers = Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections;
		Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
		Result.Session.SessionSettings.Get(FName("SESSION_DISPLAY_NAME"), Info.DisplayName);
		Result.Session.SessionSettings.Get(FName("SCORE_LIMIT"), Info.ScoreLimit);

		// Add to the Output Array
		Out.Add(Info);
	}
	
	// Return the Output Array
	return Out;
}
