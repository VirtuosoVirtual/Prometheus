// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "PrometheusCharacter.generated.h"

UCLASS(config=Game)
class APrometheusCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	APrometheusCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	//Collection Volume
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Energy)
	class USphereComponent* CollectionSphere;

	//Energy Level of characters
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
	float EnergyLevel;

	//Energy multiplier for the speed of the character 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
	float SpeedFactor;

	//Baseline speed of characters
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
	float BaseSpeed;

	virtual void Tick(float DeltaSeconds) override;

protected:

	//Called when we press a key, to collect a food object inside the SphereComponent
	UFUNCTION(BlueprintCallable, Category = Energy)
	void CollectFood();

	//Called by CollectFood() to use the Blueprinted EnergyGain functionality
	UFUNCTION(BlueprintImplementableEvent, Category = Energy)
	void EnergyGain(float FoodEnergy);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

