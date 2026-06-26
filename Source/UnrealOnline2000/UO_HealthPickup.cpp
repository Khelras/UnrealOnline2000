// Fill out your copyright notice in the Description page of Project Settings.


#include "UO_HealthPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "UnrealOnline2000Character.h"

// Sets default values
AUO_HealthPickup::AUO_HealthPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // Replication
    bReplicates = true;

    // Health Pickup Circle Component
    OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
    OverlapSphere->SetSphereRadius(80.f);
    OverlapSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    RootComponent = OverlapSphere;

    // Health Pickup Circle Mesh
    PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
    PickupMesh->SetupAttachment(RootComponent);
    PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AUO_HealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
    if (HasAuthority() == true)
    {
        OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AUO_HealthPickup::OnOverlapBegin);
    }
}

void AUO_HealthPickup::OnRep_IsActive()
{
    // Show or Hide the Mesh based on its Active State
    PickupMesh->SetVisibility(bIsActive);
    OverlapSphere->SetCollisionEnabled(bIsActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void AUO_HealthPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority() || !bIsActive) return;

    AUnrealOnline2000Character* Character = Cast<AUnrealOnline2000Character>(OtherActor);
    if (!Character) return;

    // Only Heal if not already at Full Health
    if (Character->GetHealth() >= Character->GetMaxHealth()) return;

    // Apply Heal on the Server, Health is replicated so Clients see it
    Character->Heal(FMath::Clamp(Character->GetHealth() + HealAmount, 0.0f, Character->GetMaxHealth()));

    // Deactivate pickup
    bIsActive = false;
    OnRep_IsActive(); // Manually call for Server-Local Player

    // Multicast any VFX/SFX
    Multicast_PlayPickupEffect();

    // Schedule respawn
    GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AUO_HealthPickup::Respawn, RespawnTime, false);
}

void AUO_HealthPickup::Respawn()
{
    bIsActive = true;
    OnRep_IsActive(); // Server-Local Update
    Multicast_PlayRespawnEffect_Implementation(); // SFX
}

void AUO_HealthPickup::Multicast_PlayPickupEffect_Implementation()
{
    OnPickupCollected();
}

void AUO_HealthPickup::Multicast_PlayRespawnEffect_Implementation()
{
    OnRespawn();
}
// Called every frame
void AUO_HealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUO_HealthPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AUO_HealthPickup, bIsActive);
}