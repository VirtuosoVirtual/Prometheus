// Fill out your copyright notice in the Description page of Project Settings.

#include "Prometheus.h"
#include "SpawnVolume.h"
#include "Pickup.h"


// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create a simple StaticMeshComponent to represent the pickup within the level
	WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));

	//Set the StaticMeshComponent as the Root Component
	RootComponent = WhereToSpawn;

	//Set the spawn delay range and get the first spawn delay
	SpawnDelayRangeLow = 1.0f;
	SpawnDelayRangeHigh = 5.0f;
	SpawnDelay = GetRandomSpawnDelay();
}

void ASpawnVolume::SpawnPickup()
{
	//If we have something to spawn
	if (WhatToSpawn != NULL)
	{
		//Check for a valid world
		UWorld* const World = GetWorld();
		if (World)
		{
			//Set the spawn parameters
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			//Get a random location to spawn at
			FVector SpawnLocation = GetRandomPointInVolume();

			//Get a random rotation for the spawned item
			FRotator SpawnRotation;
			SpawnRotation.Yaw = FMath::FRand() * 360.0f;
			SpawnRotation.Pitch = FMath::FRand() * 360.0f;
			SpawnRotation.Roll = FMath::FRand() * 360.0f;

			//Spawn the pickup
			APickup* const SpawnedPickup = World->SpawnActor<APickup>(WhatToSpawn->GetClass(), SpawnLocation, SpawnRotation, SpawnParams);
		}
	}
}

float ASpawnVolume::GetRandomSpawnDelay()
{
	//Get a random float that falls within the spawn delay range
	return FMath::FRandRange(SpawnDelayRangeLow, SpawnDelayRangeHigh);
}

FVector ASpawnVolume::GetRandomPointInVolume()
{
	FVector RandomLocation;
	float MinX, MinY, MinZ;
	float MaxX, MaxY, MaxZ;

	FVector Origin;
	FVector BoxExtent;

	//Get the SpawnVolume's Origin and Box Extent
	Origin = WhereToSpawn->Bounds.Origin;
	BoxExtent = WhereToSpawn->Bounds.BoxExtent;

	//Calculate the minimum X,Y & Z
	MinX = Origin.X - BoxExtent.X / 2.0f;
	MinY = Origin.Y - BoxExtent.Y / 2.0f;
	MinZ = Origin.Z - BoxExtent.Z / 2.0f;

	//Calculate the maximum X,Y & Z
	MaxX = Origin.X + BoxExtent.X / 2.0f;
	MaxY = Origin.Y + BoxExtent.Y / 2.0f;
	MaxZ = Origin.Z + BoxExtent.Z / 2.0f;

	//The random spawn location will fall between the min and max X,Y and Z
	RandomLocation.X = FMath::FRandRange(MinX, MaxX);
	RandomLocation.Y = FMath::FRandRange(MinY, MaxY);
	RandomLocation.Z = FMath::FRandRange(MinZ, MaxZ);

	//Return the random spawn location
	return RandomLocation;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	//If spawning is not enabled, do nothing
	if (!bSpawningEnabled)
	{
		return;
	}

	Super::Tick(DeltaTime);

	//Always add delta time to spawn time
	SpawnTime += DeltaTime;

	bool bShouldSpawn = (SpawnTime > SpawnDelay);

	if (bShouldSpawn)
	{
		SpawnPickup();

		//Deduct spawn delay from the accumulated time value
		SpawnTime -= SpawnDelay;

		SpawnDelay = GetRandomSpawnDelay();
	}

}

void ASpawnVolume::EnableSpawning()
{
	bSpawningEnabled = true;
}

void ASpawnVolume::DisableSpawning()
{
	bSpawningEnabled = false;
}

