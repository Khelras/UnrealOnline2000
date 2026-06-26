// Fill out your copyright notice in the Description page of Project Settings.


#include "UO_HillZone.h"
#include "UO_PlayerState.h"
#include "UO_GameMode_Match.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Character.h"

// Sets default values
AUO_HillZone::AUO_HillZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Replication
    bReplicates = true;

    // Hill Zone Sphere Component
    HillSphere = CreateDefaultSubobject<USphereComponent>(TEXT("HillSphere"));
    HillSphere->SetSphereRadius(HillRadius);
    HillSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    RootComponent = HillSphere;
    
    // Hill Zone Mesh Component
    HillMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HillMesh"));
    HillMesh->SetupAttachment(RootComponent);
    HillMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AUO_HillZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUO_HillZone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Only Server awards the Score
    if (HasAuthority() == false) return;

    APlayerState* PlayerOnHill = FindPlayerOnHill();
    CapturingPlayerState = PlayerOnHill; // This triggers OnRep on clients

    if (CapturingPlayerState)
    {
        if (Cast<AUO_GameMode_Match>(GetWorld()->GetAuthGameMode())->IsMatchInProgress() == false) return;

        ScoreAccumulator += DeltaTime;
        if (ScoreAccumulator >= ScoreInterval)
        {
            ScoreAccumulator = 0.0f;

            // Cast to your PlayerState and add score
            AUO_PlayerState* Player = Cast<AUO_PlayerState>(CapturingPlayerState);
            if (Player) Player->AddHillScore(1);
        }
    }
    else
    {
        ScoreAccumulator = 0.0f;
    }

}

APlayerState* AUO_HillZone::FindPlayerOnHill() const
{
    TArray<AActor*> OverlappingActors;
    HillSphere->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

    APlayerState* Found = nullptr;
    int32 ValidCount = 0;

    for (AActor* Actor : OverlappingActors)
    {
        ACharacter* Char = Cast<ACharacter>(Actor);
        if (Char && Char->IsPendingKillPending() == false && Char->GetPlayerState())
        {
            ValidCount++;
            if (ValidCount == 1) Found = Char->GetPlayerState();
        }
    }

    // Only return a Player if EXACTLY ONE Player is on the Hil
    return (ValidCount == 1) ? Found : nullptr;
}

void AUO_HillZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AUO_HillZone, CapturingPlayerState);
}