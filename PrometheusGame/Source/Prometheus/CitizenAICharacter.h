// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "CitizenAICharacter.generated.h"

UENUM(BlueprintType)
enum class EBotBehaviorType : uint8
{
	/* Does not move, remains in place until a player is spotted */
	PASSIVE,

	/* Patrols a region until the AI finds an area of interest */
	SEARCH_FOOD,

	/* When the AI has seen a food object*/
	FOUND_FOOD,

	/* No longer hungry, just moving around*/
	PATROLLING
};

UCLASS()
class PROMETHEUS_API ACitizenAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACitizenAICharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	//Collection Volume
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	class USphereComponent* CollectionSphere;

	//Energy Level of AI characters
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy")
	float EnergyLevel;

	//Decay Rate for the Energy Level of the AI character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy")
	float DecayRate;

	//Energy multiplier for the speed of the AI character 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy")
	float SpeedFactor;

	//Baseline speed of AI character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Energy")
	float BaseSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bSensedFood;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sight")
	float SightRangeValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sight")
	float PeripheralRangeValue;

	//Get the current Energy Level
	float GetEnergyLevel();

	//Called when the AI character needs to collect food, to collect a food object inside the SphereComponent
	UFUNCTION(BlueprintCallable, Category = Energy)
	void CollectFood(ACupcakePickup* TestFood);

protected:
	//Called by CollectFood() to use the Blueprinted EnergyGain functionality
	UFUNCTION(BlueprintImplementableEvent, Category = Energy)
	void EnergyGain(float FoodEnergy);

	/* Triggered by pawn sensing component when a pawn is spotted */
	/* When using functions as delegates they need to be marked with UFUNCTION(). We assign this function to FSeePawnDelegate */
	UFUNCTION()
	void OnSeeFood(AActor* FoodActor);

public:
	/* The bot behavior we want this bot to execute, (passive/patrol) by specifying EditAnywhere we can edit this value per-instance when placed on the map. */
	UPROPERTY(EditAnywhere, Category = "AI")
	EBotBehaviorType BotType;

	/* The thinking part of the brain, steers our zombie and makes decisions based on the data we feed it from the Blackboard */
	/* Assigned at the Character level (instead of Controller) so we may use different zombie behaviors while re-using one controller. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	/* Change default bot type during gameplay */
	void SetBotType(EBotBehaviorType NewType);
};


