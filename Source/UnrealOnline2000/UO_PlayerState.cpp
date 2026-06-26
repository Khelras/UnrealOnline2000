// Fill out your copyright notice in the Description page of Project Settings.


#include "UO_PlayerState.h"
#include "UO_GameMode_Match.h"
#include "Net/UnrealNetwork.h"

void AUO_PlayerState::GiveElimination()
{
	// Get the Match Game Mode
	if (AUO_GameMode_Match* GameMode = Cast<AUO_GameMode_Match>(GetWorld()->GetAuthGameMode()))
	{
		// Check if the Match is in Progress
		if (GameMode->IsMatchInProgress() == true)
		{
			// Increase Eliminations
			Eliminations++;

			// Check Win Conditions
			GameMode->CheckForWinner();
		}
	}
}

void AUO_PlayerState::AddHillScore(int32 Amount)
{
	if (HasAuthority() == false) return;
	HillScore += Amount;

	// Check for a Win
	if (AUO_GameMode_Match* GameMode = GetWorld()->GetAuthGameMode<AUO_GameMode_Match>())
	{
		GameMode->CheckForWinner();
	}
}

void AUO_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUO_PlayerState, Eliminations)
	DOREPLIFETIME(AUO_PlayerState, HillScore)
}