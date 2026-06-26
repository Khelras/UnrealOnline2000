// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UO_HealthPickup.generated.h"

UCLASS()
class UNREALONLINE2000_API AUO_HealthPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUO_HealthPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
    class USphereComponent* OverlapSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup")
    class UStaticMeshComponent* PickupMesh;

    // Replicated — OnRep drives visibility for all clients
    UPROPERTY(ReplicatedUsing = OnRep_IsActive, BlueprintReadOnly, Category="Pickup")
    bool bIsActive = true;

    UFUNCTION()
    void OnRep_IsActive();

    UPROPERTY(EditDefaultsOnly, Category="Pickup")
    float HealAmount = 50.f;

    UPROPERTY(EditDefaultsOnly, Category="Pickup")
    float RespawnTime = 10.f;

    FTimerHandle RespawnTimerHandle;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    void Respawn();

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_PlayPickupEffect();
    
    UFUNCTION(BlueprintImplementableEvent, Category="Pickup")
    void OnPickupCollected();

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_PlayRespawnEffect();

    UFUNCTION(BlueprintImplementableEvent, Category="Pickup")
    void OnRespawn();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
