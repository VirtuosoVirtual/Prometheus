// Fill out your copyright notice in the Description page of Project Settings.

#include "Prometheus.h"
#include "CitizenAICharacter.h"
#include "CitizenAIController.h"
#include "CupcakePickup.h"
#include "PrometheusGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACitizenAICharacter::ACitizenAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set the sight range for the AI character
	SightRangeValue = 1000.0f;

	//Set the peripheral vision for the AI character
	PeripheralRangeValue = 60.0f;

	//Set the base power for the AI character
	EnergyLevel = 2000.0f;
	//Set the decay rate for the AI character
	DecayRate = 0.01f;
	//Set the depencence of speed on the power level
	SpeedFactor = 0.2f;
	BaseSpeed = 10.0f;

	//Create our food collection sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->AttachTo(RootComponent);
	CollectionSphere->SetSphereRadius(200.0f);
}

// Called when the game starts or when spawned
void ACitizenAICharacter::BeginPlay()
{
	Super::BeginPlay();
	/* This is the earliest moment we can bind our delegates to the component */
	/*
	if (AIPerceptionComp)
	{
		//PawnSensingComp->OnSeePawn.AddDynamic(this, &ASZombieCharacter::OnSeePlayer);
	}
	*/
}

// Called every frame
void ACitizenAICharacter::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

	GetCharacterMovement()->MaxWalkSpeed = SpeedFactor * EnergyLevel + BaseSpeed;

	//ACitizenAICharacter* MyCharacter = Cast<ACitizenAICharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	
	//If the AI character still has power
	if (GetEnergyLevel() > 0.05f)
	{
		int testNumber = 0;
		//Decrease the AI character's power
		EnergyLevel = FMath::FInterpTo(EnergyLevel, 0.0f, DeltaTime, DecayRate);
	}

	//Create 
	ACitizenAIController* AIController = Cast<ACitizenAIController>(GetController());
	if (AIController)
	{
		bSensedFood = false;
		/* Reset */
		AIController->SetFoodTarget(nullptr);
	}
	
	//Set an AI death condition in the future
}

// Called to bind functionality to input
void ACitizenAICharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void ACitizenAICharacter::CollectFood()
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
				//Store the Food Enegry for adding the Citizen's Energy
				FoodEnergy = FoodEnergy + TestFood->EnergyLevel;
				//Call the battery's OnPickedUp function
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

void ACitizenAICharacter::OnSeeFood(AActor* FoodActor)
{
	/*
	if (!IsAlive())
	{
		return;
	}

	if (!bSensedTarget)
	{
		BroadcastUpdateAudioLoop(true);
	}
	*/

	/* Keep track of the time the player was last sensed in order to clear the target */
	/*
	LastSeenTime = GetWorld()->GetTimeSeconds();
	*/
	bSensedFood = true;

	ACitizenAIController* AIController = Cast<ACitizenAIController>(GetController());
	ACupcakePickup* SensedFood = Cast<ACupcakePickup>(FoodActor);
	if (AIController && SensedFood)
	{
		AIController->SetFoodTarget(SensedFood);
	}
}

float ACitizenAICharacter::GetEnergyLevel()
{
	return EnergyLevel;
}

void ACitizenAICharacter::SetBotType(EBotBehaviorType NewType)
{
	BotType = NewType;

	ACitizenAIController* AIController = Cast<ACitizenAIController>(GetController());
	if (AIController)
	{
		AIController->SetBlackboardBotType(NewType);
	}

	//BroadcastUpdateAudioLoop(bSensedTarget);
}

