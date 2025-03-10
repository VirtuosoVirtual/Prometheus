// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Prometheus.h"
#include "PrometheusCharacter.h"
#include "CupcakePickup.h"
#include "PrometheusGameMode.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// APrometheusCharacter

APrometheusCharacter::APrometheusCharacter()
{
	//Set the base power for the character
	EnergyLevel = 2000.0f;
	//Set the depencence of speed on the power level
	SpeedFactor = 0.2f;
	BaseSpeed = 10.0f;

	//Create our food collection sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->AttachTo(RootComponent);
	CollectionSphere->SetSphereRadius(150.0f);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void APrometheusCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAction("CollectPickups", IE_Pressed, this, &APrometheusCharacter::CollectFood);

	InputComponent->BindAxis("MoveForward", this, &APrometheusCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APrometheusCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &APrometheusCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &APrometheusCharacter::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &APrometheusCharacter::TouchStarted);
	InputComponent->BindTouch(IE_Released, this, &APrometheusCharacter::TouchStopped);
}


void APrometheusCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	// jump, but only on the first touch
	if (FingerIndex == ETouchIndex::Touch1)
	{
		Jump();
	}
}

void APrometheusCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (FingerIndex == ETouchIndex::Touch1)
	{
		StopJumping();
	}
}

void APrometheusCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APrometheusCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APrometheusCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APrometheusCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void APrometheusCharacter::CollectFood()
{
	float FoodEnergy = 0.0f;

	APrometheusGameMode* MyGameMode = Cast<APrometheusGameMode>(UGameplayStatics::GetGameMode(this));

	//If we are currently playing
	if (MyGameMode->GetCurrentState() == EPrometheusPlayState::EPlaying)
	{
		//Get all overlapping Actors and store them in a CollectedActors array
		TArray<AActor*> CollectedActors;
		CollectionSphere->GetOverlappingActors(CollectedActors);

		//For each actor collected
		for (int iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
		{
			//Cast the collected Actor to a ABatteryPickup
			ACupcakePickup* const TestFood = Cast<ACupcakePickup>(CollectedActors[iCollected]);

			//if the cast is successful, and the battery is valid and active
			if (TestFood && !TestFood->IsPendingKill() && TestFood->bIsActive)
			{
				//Store its battery power for adding the character's power
				FoodEnergy = FoodEnergy + TestFood->EnergyLevel;
				//Call the Cupcake's OnPickedUp function
				TestFood->OnPickedUp();
				//Deactivate the food object
				TestFood->bIsActive = false;
			}

			if (FoodEnergy > 0.0f)
			{
				//Call the Blueprinted implementation of EnergyGain with the total food energy as input
				EnergyGain(FoodEnergy);
			}
		}
	}
}

void APrometheusCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	GetCharacterMovement()->MaxWalkSpeed = SpeedFactor * EnergyLevel + BaseSpeed;
}