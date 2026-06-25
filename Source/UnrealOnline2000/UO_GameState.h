// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "UO_GameState.generated.h"

class AUO_PlayerState;

/**
 * 
 */
UCLASS()
class UNREALONLINE2000_API AUO_GameState : public AGameState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Replicated)
	AUO_PlayerState* WinningPlayer = nullptr;

public:
	void SetWinner(AUO_PlayerState* _Winner);

	UFUNCTION(BlueprintPure)
	AUO_PlayerState* GetWinner();
};
