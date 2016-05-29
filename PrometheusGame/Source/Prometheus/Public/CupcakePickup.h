// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "CupcakePickup.generated.h"

/**
 * 
 */
UCLASS()
class PROMETHEUS_API ACupcakePickup : public APickup
{
	GENERATED_BODY()

public:
	ACupcakePickup();

	//Sets the amount of power of the battery to give to the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Energy)
	float EnergyLevel;

	void OnPickedUp_Implementation() override;
};
