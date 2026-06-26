// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealOnline2000.h"
#include "Engine/GameInstance.h"
#include "UO_GameInstance.generated.h"

// Struct for King of the Hill Session Settings
USTRUCT(BlueprintType)
struct FKOTHSessionSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Session")
	FString SessionName = TEXT("King of the Hill");
	
	// Points to Win
	UPROPERTY(BlueprintReadWrite, Category="Session")
	int32 ScoreLimit = 100;

	// Max Players
	UPROPERTY(BlueprintReadWrite, Category="Session")
	int32 MaxPlayers = 4;
};

// Struct for Displaying Find Server Information in the Main Menu
USTRUCT(BlueprintType)
struct FSessionDisplayInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString DisplayName;
	UPROPERTY(BlueprintReadOnly) int32 ScoreLimit;
	UPROPERTY(BlueprintReadOnly) int32 MaxPlayers;
	UPROPERTY(BlueprintReadOnly) int32 CurrentPlayers;
	UPROPERTY(BlueprintReadOnly) int32 ResultIndex;
};

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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionsFound);

	UPROPERTY(BlueprintAssignable, Category = "Sessions")
	FOnSessionsFound OnSessionsFound;

	void OnCreateSessionComplete(FName _sessionName, bool _bSuccess);
	void OnFindSessionsComplete(bool _bSuccess);
	void OnJoinSessionComplete(FName _sessionName, EOnJoinSessionCompleteResult::Type _result);
	void OnDestroySessionComplete(FName _sessionName, bool _bSuccess);

	UFUNCTION()
	void NetworkFailureOccurred(UWorld* _world, UNetDriver* _netDriver, ENetworkFailure::Type _failureType, const FString& _errorString);

	FKOTHSessionSettings HostSettings;
	TSharedPtr<FOnlineSessionSettings> SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	bool HostSession(FKOTHSessionSettings _HostSettings, bool _bIsLAN);
	void FindSessions(TSharedPtr<const FUniqueNetId> _userID, bool _bIsLAN);
	bool JoinSession(TSharedPtr<const FUniqueNetId> _userID, FName _sessionName, const FOnlineSessionSearchResult& _searchResult);
	
	UFUNCTION(BlueprintCallable)
	void StartGame(FKOTHSessionSettings _HostSettings, bool _bLAN);

	UFUNCTION(BlueprintCallable)
	void FindGames(bool _bLAN);

	UFUNCTION(BlueprintCallable)
	void JoinGame(int _iServerIndex);

	UFUNCTION(BlueprintCallable)
	void DestroySession();

public:
	UFUNCTION(BlueprintCallable, Category="Sessions")
	TArray<FSessionDisplayInfo> GetSessionDisplayInfos();
};
