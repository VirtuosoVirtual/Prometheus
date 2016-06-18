// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Prometheus.h"
#include "PrometheusGameMode.h"
#include "PrometheusCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PrometheusHUD.h"

APrometheusGameMode::APrometheusGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//Set the default HUD class
	HUDClass = APrometheusHUD::StaticClass();

	//Base decay rate
	DecayRate = 0.001f;
}

void APrometheusGameMode::Tick(float DeltaSeconds)
{
	APrometheusCharacter* MyCharacter = Cast<APrometheusCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	//If the character still has power
	if (MyCharacter->EnergyLevel > 0.05)
	{
		//Decrease the character's power
		MyCharacter->EnergyLevel = FMath::FInterpTo(MyCharacter->EnergyLevel, 0.0f, DeltaSeconds, DecayRate);
	}
	else
	{
		SetCurrentState(EPrometheusPlayState::EGameOver);
	}
}

void APrometheusGameMode::SetCurrentState(EPrometheusPlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(NewState);
}

void APrometheusGameMode::HandleNewState(EPrometheusPlayState NewState)
{
	switch (NewState)
	{
	//Where we are playing, the spawn volumes can spawn
	case EPrometheusPlayState::EPlaying:
	{
		for (ASpawnVolume* Volume : SpawnVolumeActors)
		{
			Volume->EnableSpawning();
		}
	}
	break;

	//If the game is over, the spawn volume cannot spawn
	case EPrometheusPlayState::EGameOver:
	{
		for (ASpawnVolume* Volume : SpawnVolumeActors)
		{
			Volume->DisableSpawning();
		}
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		PlayerController->SetCinematicMode(true, false, false, true, true);
	}
	break;

	case EPrometheusPlayState::EUnknown:
	default:
		break;
		//Do nothing
	}
}

void APrometheusGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Find all spawn volume actors
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundActors);

	for (auto Actor : FoundActors)
	{
		ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
		if (SpawnVolumeActor)
		{
			SpawnVolumeActors.Add(SpawnVolumeActor);
		}
	}
	SetCurrentState(EPrometheusPlayState::EPlaying);
}