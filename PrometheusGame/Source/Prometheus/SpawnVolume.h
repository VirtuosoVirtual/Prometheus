// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class PROMETHEUS_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpawnVolume();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	//Box Component to specify the spawning area within the level 
	UPROPERTY(VisibleInstanceOnly, Category = Spawning)
		UBoxComponent *WhereToSpawn;

	//The pickup to spawn
	UPROPERTY(EditAnywhere, Category = Spawning)
	class APickup* WhatToSpawn;

	//Minimum Spawn Delay
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Spawning)
		float SpawnDelayRangeLow;

	//Maximum Spawn Delay
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Spawning)
		float SpawnDelayRangeHigh;

	//Finds a random point within the Box Component
	UFUNCTION(BlueprintPure, Category = Spawning)
		FVector GetRandomPointInVolume();

	void EnableSpawning();

	void DisableSpawning();

private:
	//Whether or not spawning is enabled
	bool bSpawningEnabled;

	//Calculates a random spawn delay
	float GetRandomSpawnDelay();

	//The current spawn delay 
	float SpawnDelay;

	//Handles the spawning of a new pickup
	void SpawnPickup();

	//The timer for when to spawn the pickup
	float SpawnTime;
};
