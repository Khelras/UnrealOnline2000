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
	// Called whe the Match Game State enters WaitingPostMatch
	virtual void HandleMatchHasEnded() override;

	// Timer to Delay the Game Restart
	FTimerHandle MatchRestartDelayTimer;

public:
	void RespawnPlayer(APlayerController* _PlayerToRespawn);
	void CheckForWinner();
};
