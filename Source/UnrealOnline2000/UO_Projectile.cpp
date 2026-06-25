// Fill out your copyright notice in the Description page of Project Settings.


#include "UO_Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "UnrealOnline2000Character.h"
#include "UO_PlayerState.h"

// Sets default values
AUO_Projectile::AUO_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// Create the Projectile Movement Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));

	// Create the Sphere Collision and make it the Root Component
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->OnComponentHit.AddDynamic(this, &AUO_Projectile::OnProjectileHit);
	RootComponent = SphereCollision;

	// Create the Projectile Mesh, turn off its Collision, and attach it to the Sphere Collision
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AUO_Projectile::BeginPlay()
{
	Super::BeginPlay();
	
	// Prevent the Projectile from hitting the Owner Character
	SphereCollision->IgnoreActorWhenMoving(GetOwner(), true);
	if (AUnrealOnline2000Character* OwnerCharacter = Cast <AUnrealOnline2000Character>(GetOwner()))
	{
		OwnerCharacter->MoveIgnoreActorAdd(this);

		// Set the Player State
		OwnerPlayerState = Cast<AUO_PlayerState>(OwnerCharacter->GetPlayerState());
	}
}

void AUO_Projectile::OnProjectileHit(UPrimitiveComponent* _thisHitComp, AActor* _otherActor,
	UPrimitiveComponent* _otherHitComp, FVector _normalImpulse, const FHitResult& _hitResult)
{
	if (HasAuthority() == true)
	{
		if (AUnrealOnline2000Character* HitCharacter = Cast<AUnrealOnline2000Character>(_otherActor))
		{
			HitCharacter->UO_TakeDamage(Damage, OwnerPlayerState);
		}

		// Destroy the Projectile after hitting something
		Destroy();
	}
}

// Called every frame
void AUO_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

