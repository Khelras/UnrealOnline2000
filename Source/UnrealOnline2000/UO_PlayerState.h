// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UO_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UNREALONLINE2000_API AUO_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	// The current amount of Eliminations the Player has
	UPROPERTY(Replicated)
	int32 Eliminations = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Score")
	int32 HillScore = 0;

public:
	// Adds an Elimination
	void GiveElimination();
	
	// Returns the Amount of Eliminations
	UFUNCTION(BlueprintPure)
	int32 GetEliminations() { return Eliminations; };

	void AddHillScore(int32 Amount);
	int32 GetHillScore() const { return HillScore; };
};
