// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SwordplayCharacter.h"
#include "SwordplayProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ASwordplayCharacter

ASwordplayCharacter::ASwordplayCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	
	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

}

void ASwordplayCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(false, true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASwordplayCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASwordplayCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASwordplayCharacter::StopJump);

	// Bind crouch events
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASwordplayCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASwordplayCharacter::StopCrouch);

	// Bind Circle event
	PlayerInputComponent->BindAction("Circle", IE_Pressed, this, &ASwordplayCharacter::SpawnCircle);



	PlayerInputComponent->BindAction("ChangeMode", IE_Pressed, this, &ASwordplayCharacter::ChangeGender);

	// Bind fire event
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASwordplayCharacter::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ASwordplayCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASwordplayCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASwordplayCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASwordplayCharacter::LookUpAtRate);
}

void ASwordplayCharacter::ChangeGender()
{
	BoyGirlMode = !BoyGirlMode;
	GenderChanged();
}


void ASwordplayCharacter::FencingRange()
{
}

void ASwordplayCharacter::CircleEnded()
{
	GLog->Log("Circle ENDED");
	circleMode = false;
}

void ASwordplayCharacter::SpawnCircle()
{
	if (circleMode)
	{
		circleActor->SelectionMade();
	}
	else
	{
		if (fencingTarget)
		{
			GLog->Log("Spawning circle...");
			// try and create circle
			if (CircleActorBP != NULL)
			{
				UWorld* const World = GetWorld();
				if (World != NULL)
				{
					FRotator SpawnRotation = GetActorRotation();
					FVector SpawnLocation = GetActorLocation() + (FirstPersonCameraComponent->GetForwardVector() * 275);

					SpawnRotation.Pitch += 7;
					SpawnLocation.Z = GetActorLocation().Z - 30;

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

					// spawn the circle
					circleActor = World->SpawnActor<ACircleActor>(CircleActorBP, SpawnLocation, SpawnRotation, ActorSpawnParams);
				}
			}
			circleActor->PassCharRef(this, BoyGirlMode);
			circleMode = true;
			CircleCreated(circleActor);
			curSelection = 0;
		}
	}
}

void ASwordplayCharacter::StartCrouch()
{
	if (circleMode)
	{

	}
	else
	{
		if (!BoyGirlMode)
			Crouch();
	}
}

void ASwordplayCharacter::StopCrouch()
{
	UnCrouch();
}

void ASwordplayCharacter::StartJump()
{
	if (circleMode)
	{
		//Selection made
		circleActor->SelectionMade();
	}
	else
	{
		if (!BoyGirlMode)
			Jump();
	}
}

void ASwordplayCharacter::StopJump()
{
	StopJumping();
}

void ASwordplayCharacter::OnFire()
{
	/* Keeping for reference
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<ASwordplayProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}*/
}

void ASwordplayCharacter::MoveForward(float Value)
{
	if (circleMode)
	{

	}
	else
	{
		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorForwardVector(), Value);
		}
	}
}

void ASwordplayCharacter::MoveRight(float Value)
{
	if (circleMode)
	{
		//Switch selection
		if (Value != 0.0f)
		{
			curSelection = Value;
			circleActor->HighlightSelection(Value);
		}
	}
	else
	{
		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
}

void ASwordplayCharacter::TurnAtRate(float Rate)
{
	if (circleMode)
	{

	}
	else
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ASwordplayCharacter::LookUpAtRate(float Rate)
{
	if (circleMode)
	{

	}
	else
	{
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}