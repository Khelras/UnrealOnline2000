// Fill out your copyright notice in the Description page of Project Settings.


#include "UO_GameState.h"
#include "Net/UnrealNetwork.h"

void AUO_GameState::SetWinner(AUO_PlayerState* _Winner)
{
	WinningPlayer = _Winner;
}

AUO_PlayerState* AUO_GameState::GetWinner()
{
	return WinningPlayer;
}

void AUO_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUO_GameState, WinningPlayer);
}
