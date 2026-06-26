// Fill out your copyright notice in the Description page of Project Settings.


#include "UO_GameState.h"
#include "UO_HillZone.h"
#include "UO_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

TArray<FString> AUO_GameState::GetConnectedPlayerNames() const
{
	TArray<FString> Names;
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState) Names.Add(PlayerState->GetPlayerName());
	}

	return Names;
}

TArray<FPlayerLeaderboardEntry> AUO_GameState::GetTopPlayers() const
{
    TArray<FPlayerLeaderboardEntry> Entries;

    // Find the Hill Zone to know who's currently on the Hill
    AActor* HillActor = UGameplayStatics::GetActorOfClass(GetWorld(), AUO_HillZone::StaticClass());
    AUO_HillZone* HillZone = Cast<AUO_HillZone>(HillActor);
    APlayerState* CapturingPS = HillZone ? HillZone->GetCapturingPlayerState() : nullptr;

    // Build the Entries from the PlayerArray
    for (APlayerState* PS : PlayerArray)
    {
        // Cast to our Player State
        AUO_PlayerState* KPS = Cast<AUO_PlayerState>(PS);
        if (!KPS) continue;

        FPlayerLeaderboardEntry Entry;
        Entry.PlayerName = KPS->GetPlayerName();
        Entry.Score = KPS->GetHillScore();
        Entry.bIsOnHill = (KPS == CapturingPS);
        Entries.Add(Entry);
    }

    // Sort Descending by Score
    Entries.Sort([](const FPlayerLeaderboardEntry& A, const FPlayerLeaderboardEntry& B) {
        return A.Score > B.Score;
    });

    return Entries;
}

void AUO_GameState::SetWinner(AUO_PlayerState* _Winner)
{
	WinningPlayer = _Winner;
}

void AUO_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUO_GameState, WinningPlayer);
	DOREPLIFETIME(AUO_GameState, ScoreLimit);
}
