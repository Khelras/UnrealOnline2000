// Fill out your copyright notice in the Description page of Project Settings.


#include "UO_GameInstance.h"
#include <Online/OnlineSessionNames.h>

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
		FString LevelOptions = "Lvl_ThirdPerson?listen";
		if (SessionSettings->bIsLANMatch == true) LevelOptions.Append("?bIsLanMatch=1");
		GetWorld()->ServerTravel(LevelOptions);
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
}

void UUO_GameInstance::OnJoinSessionComplete(FName _sessionName, EOnJoinSessionCompleteResult::Type _result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
			FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *_sessionName.ToString(), static_cast<int32>(_result)));
	if (SessionInterface.IsValid() == false) return;

	APlayerController* const PlayerController = GetFirstLocalPlayerController();

	FString TravelURL;

	if (PlayerController && SessionInterface->GetResolvedConnectString(_sessionName, TravelURL))
	{
		PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
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
	// Destroy SEssion on Network Failure
	DestroySession();
}

bool UUO_GameInstance::HostSession(TSharedPtr<const FUniqueNetId> _userID, FName _sessionName, bool _bIsLAN, bool _bIsPresence, int32 _maxNumPlayers)
{
	if (SessionInterface.IsValid() == false) return false;
	if (_userID.IsValid() == false) return false;

	SessionSettings = MakeShareable<FOnlineSessionSettings>(new FOnlineSessionSettings());

	// Set the Session Settings
	SessionSettings->bIsLANMatch = _bIsLAN;
	SessionSettings->bUsesPresence = _bIsPresence;
	SessionSettings->NumPublicConnections = _maxNumPlayers;
	SessionSettings->NumPrivateConnections = 0;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	SessionSettings->bUseLobbiesIfAvailable = true;

	// Set the Session Map
	SessionSettings->Set(SEARCH_KEYWORDS, FString("Custom"), EOnlineDataAdvertisementType::ViaOnlineService);

	// Create the Session
	return SessionInterface->CreateSession(*_userID, _sessionName, *SessionSettings);
}

void UUO_GameInstance::FindSessions(TSharedPtr<const FUniqueNetId> _userID, bool _bIsLAN)
{
	if (SessionInterface.IsValid() == false) OnFindSessionsComplete(false);
	if (_userID.IsValid() == false) OnFindSessionsComplete(false);

	SessionSearch = MakeShareable(new FOnlineSessionSearch);

	// Set the Session Search Settings
	SessionSearch->bIsLanQuery = _bIsLAN;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->PingBucketSize = 50;
	
	// Set the Session Search Query Settings
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

void UUO_GameInstance::StartGame(bool _bLAN)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	HostSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, _bLAN, true, 4);
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
