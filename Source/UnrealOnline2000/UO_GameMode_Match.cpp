// Fill out your copyright notice in the Description page of Project Settings.


#include "UO_GameMode_Match.h"
#include "UO_GameState.h"
#include "UO_PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AUO_GameMode_Match::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Parse the ?ScoreLimit=... from the Travel URL
	FString ScoreLimitStr = UGameplayStatics::ParseOption(Options, TEXT("ScoreLimit"));
	if (ScoreLimitStr.IsEmpty() == false) ScoreLimit = FCString::Atoi(*ScoreLimitStr);

	// Clamp to sane values just in case
	ScoreLimit = FMath::Clamp(ScoreLimit, 3, 9999);
	UE_LOG(LogTemp, Log, TEXT("UO_GameMode_Match: ScoreLimit set to %d"), ScoreLimit);
}

void AUO_GameMode_Match::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AUO_GameState* MatchGameState = GetGameState<AUO_GameState>())
	{
		// Send the Score Limit to the Game State Clients so they can Read it
		MatchGameState->ScoreLimit = ScoreLimit;
	}
}

bool AUO_GameMode_Match::ReadyToStartMatch_Implementation()
{
	// Never Auto-Start as the Host must press the Start Game Button
	return false;
}

void AUO_GameMode_Match::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	// Restart after a Delay of 5 Seconds
	GetWorldTimerManager().SetTimer(MatchRestartDelayTimer, this, &AGameMode::RestartGame, 5.0f, false);
}

void AUO_GameMode_Match::HostStartMatch()
{
	// Only allow if we have at least 1 Player
	if (GetNumPlayers() >= 1)
	{
		StartMatch();
	}
}

void AUO_GameMode_Match::RespawnPlayer(APlayerController* _PlayerToRespawn)
{
	// Respawn only if a Match is in Progress
	if (IsMatchInProgress() == true)
	{
		// Collect all Player Start Locations
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

		// Fallback for when there were no Player Start Location
		if (PlayerStarts.Num() == 0)
		{
			RestartPlayer(_PlayerToRespawn);  
			return;
		}

		// Pick a random one
		AActor* ChosenStart = PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)];
		RestartPlayerAtPlayerStart(_PlayerToRespawn, ChosenStart);
	}
}

void AUO_GameMode_Match::CheckForWinner()
{
	if (AUO_GameState* UOGameState = GetGameState<AUO_GameState>())
	{
		// Check if there is a Match in Progress
		if (IsMatchInProgress() == false) return;

		// Loop through the Players in the Game
		for (auto iter : UOGameState->PlayerArray)
		{
			// Cast to Player State
			if (AUO_PlayerState* UOPlayerState = Cast<AUO_PlayerState>(iter))
			{
				// Check if this Player has met the WIn Condition
				bool bWinCondition = UOPlayerState->GetHillScore() >= ScoreLimit;
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
