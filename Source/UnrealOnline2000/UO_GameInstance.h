// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealOnline2000.h"
#include "Engine/GameInstance.h"
#include "UO_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREALONLINE2000_API UUO_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:	
	IOnlineSessionPtr SessionInterface;
	virtual void Init() override;

	void OnCreateSessionComplete(FName _sessionName, bool _bSuccess);
	void OnFindSessionsComplete(bool _bSuccess);
	void OnJoinSessionComplete(FName _sessionName, EOnJoinSessionCompleteResult::Type _result);
	void OnDestroySessionComplete(FName _sessionName, bool _bSuccess);

	UFUNCTION()
	void NetworkFailureOccurred(UWorld* _world, UNetDriver* _netDriver, ENetworkFailure::Type _failureType, const FString& _errorString);



	TSharedPtr<FOnlineSessionSettings> SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	bool HostSession(TSharedPtr<const FUniqueNetId> _userID, FName _sessionName, bool _bIsLAN, bool _bIsPresence, int32 _maxNumPlayers);
	void FindSessions(TSharedPtr<const FUniqueNetId> _userID, bool _bIsLAN);
	bool JoinSession(TSharedPtr<const FUniqueNetId> _userID, FName _sessionName, const FOnlineSessionSearchResult& _searchResult);
	
	UFUNCTION(BlueprintCallable)
	void StartGame(bool _bLAN);

	UFUNCTION(BlueprintCallable)
	void FindGames(bool _bLAN);

	UFUNCTION(BlueprintCallable)
	void JoinGame(int _iServerIndex);

	UFUNCTION(BlueprintCallable)
	void DestroySession();
};
