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
	int Eliminations = 0;

public:
	// Adds an Elimination
	void GiveElimination();
	
	// Returns the Amount of Eliminations
	UFUNCTION(BlueprintPure)
	int GetEliminations();
};
