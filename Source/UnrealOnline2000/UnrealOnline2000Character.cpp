// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealOnline2000Character.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "UnrealOnline2000.h"
#include "UO_Projectile.h"
#include "UO_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "UnrealOnline2000PlayerController.h"

AUnrealOnline2000Character::AUnrealOnline2000Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AUnrealOnline2000Character::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (HasAuthority() == true)
	{
		ControlPitch = GetControlRotation().Pitch;
	}
}

float AUnrealOnline2000Character::GetReplicatedPitch()
{
	if (IsLocallyControlled() == true)
	{
		return GetControlRotation().Pitch;
	}

	return ControlPitch;
}

void AUnrealOnline2000Character::UO_TakeDamage(float _Damage, AUO_PlayerState* _PlayerThatDealtDamage)
{
	// Take Damage
	Health -= _Damage;

	if (Health <= 0.0f)
	{
		// Clamp to 0
		Health = 0.0f;

		_PlayerThatDealtDamage->GiveElimination();

		// Die
		Die();
	}
}

void AUnrealOnline2000Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUnrealOnline2000Character::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AUnrealOnline2000Character::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUnrealOnline2000Character::Look);

		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AUnrealOnline2000Character::Attack);
	}
	else
	{
		UE_LOG(LogUnrealOnline2000, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AUnrealOnline2000Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AUnrealOnline2000Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AUnrealOnline2000Character::Attack(const FInputActionValue& Value)
{
	ServerAttack();
}

void AUnrealOnline2000Character::Die()
{
	// Respawning
	GetController<AUnrealOnline2000PlayerController>()->RespawnAfterDelay();

	Destroy();
}

void AUnrealOnline2000Character::ServerAttack_Implementation()
{
	// Spawn Tranforms
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetMesh()->GetBoneLocation("hand_l"));
	SpawnTransform.SetRotation(GetControlRotation().Quaternion());

	// Spawn Parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	// Spawn the Projection
	AUO_Projectile* SpawnedProjectile = GetWorld()->SpawnActor<AUO_Projectile>(ProjectileClass, SpawnTransform, SpawnParams);
}

void AUnrealOnline2000Character::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AUnrealOnline2000Character::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AUnrealOnline2000Character::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AUnrealOnline2000Character::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AUnrealOnline2000Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUnrealOnline2000Character, ControlPitch);
	DOREPLIFETIME(AUnrealOnline2000Character, Health);
}
