// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "UO_GameMode_Match.generated.h"

/**
 * 
 */
UCLASS()
class UNREALONLINE2000_API AUO_GameMode_Match : public AGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual bool ReadyToStartMatch_Implementation() override;

	UPROPERTY(BlueprintReadOnly, Category="Match")
	int32 ScoreLimit = 100; // Default to 100

	// Called whe the Match Game State enters WaitingPostMatch
	virtual void HandleMatchHasEnded() override;

	// Timer to Delay the Game Restart
	FTimerHandle MatchRestartDelayTimer;

public:
	UFUNCTION(BlueprintCallable, Category="Match")
	void HostStartMatch();

	void RespawnPlayer(APlayerController* _PlayerToRespawn);
	void CheckForWinner();
};
