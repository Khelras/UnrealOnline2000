// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UO_HillZone.generated.h"

UCLASS()
class UNREALONLINE2000_API AUO_HillZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUO_HillZone();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HillZone")
	APlayerState* GetCapturingPlayerState() { return CapturingPlayerState; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="HillZone")
	class USphereComponent* HillSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="HillZone")
	class UStaticMeshComponent* HillMesh;

	// Replicated to which Player is currently Capturing the Hill
	UPROPERTY(Replicated, BlueprintReadOnly, Category="HillZone")
	class APlayerState* CapturingPlayerState;

	// Accumulator for how many Seconds a Player has been on the Hill
	float ScoreAccumulator = 0.0f;

	// Award 1 point every X seconds on hill
	UPROPERTY(EditDefaultsOnly, Category = "HillZone")
	float ScoreInterval = 1.0f; 

	UPROPERTY(EditDefaultsOnly, Category = "HillZone")
	float HillRadius = 250.0f;

	// Returns a Player is there is only ONE Player on the Hill
	APlayerState* FindPlayerOnHill() const;
};
