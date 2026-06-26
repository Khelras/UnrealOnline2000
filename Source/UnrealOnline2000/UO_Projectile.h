// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UO_Projectile.generated.h"

UCLASS()
class UNREALONLINE2000_API AUO_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUO_Projectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The Projectile Movement Component
	UPROPERTY(EditDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovement = nullptr;

	// Static Mesh Component
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ProjectileMesh = nullptr;

	// Sphere Collision Component
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* SphereCollision = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float Damage = 20.0f;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* _thisHitComp, AActor* _otherActor,
		UPrimitiveComponent* _otherHitComp, FVector _normalImpulse, const FHitResult& _hitResult);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitEffect();

	UFUNCTION(BlueprintImplementableEvent, Category="Projectile")
	void OnHit();

	UPROPERTY()
	class AUO_PlayerState* OwnerPlayerState = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
