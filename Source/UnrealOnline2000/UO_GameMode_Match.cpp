// Fill out your copyright notice in the Description page of Project Settings.


#include "UO_GameMode_Match.h"
#include "UO_GameState.h"
#include "UO_PlayerState.h"

void AUO_GameMode_Match::HandleMatchHasEnded()
{
	// Restart after a Delay of 5 Seconds
	GetWorldTimerManager().SetTimer(MatchRestartDelayTimer, this, &AGameMode::RestartGame, 5.0f, false);
}

void AUO_GameMode_Match::RespawnPlayer(APlayerController* _PlayerToRespawn)
{
	// Respawn only if a Match is in Progress
	if (IsMatchInProgress() == true)
	{
		RestartPlayer(_PlayerToRespawn);
	}
}

void AUO_GameMode_Match::CheckForWinner()
{
	if (AUO_GameState* UOGameState = GetGameState<AUO_GameState>())
	{
		// Loop through the Players in the Game
		for (auto iter : UOGameState->PlayerArray)
		{
			// Cast to Player State
			if (AUO_PlayerState* UOPlayerState = Cast<AUO_PlayerState>(iter))
			{
				// Check if this Player has met the WIn Condition
				bool bWinCondition = UOPlayerState->GetEliminations() >= 5;
				if (bWinCondition == true)
				{
					// Set the Winner
					UOGameState->SetWinner(UOPlayerState);

					// End the Match
					EndMatch();
					break;
				}
			}
		}
	}
}
