// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "UO_GameState.generated.h"

class AUO_PlayerState;

// Struct for Player Leaderboard Entry
USTRUCT(BlueprintType)
struct FPlayerLeaderboardEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FString PlayerName;
	UPROPERTY(BlueprintReadOnly) int32 Score;
	UPROPERTY(BlueprintReadOnly) bool bIsOnHill;
};

/**
 * 
 */
UCLASS()
class UNREALONLINE2000_API AUO_GameState : public AGameState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Match")
	AUO_PlayerState* WinningPlayer = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category="Match")
	TArray<FString> GetConnectedPlayerNames() const;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Match")
	int32 ScoreLimit = 100; // Default to 100

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Match")
	TArray<FPlayerLeaderboardEntry> GetTopPlayers() const;

	void SetWinner(AUO_PlayerState* _Winner);

	UFUNCTION(BlueprintPure)
	AUO_PlayerState* GetWinner() const { return WinningPlayer; };
};
