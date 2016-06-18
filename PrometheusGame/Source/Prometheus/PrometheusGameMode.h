// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "SpawnVolume.h"
#include "GameFramework/GameMode.h"
#include "PrometheusGameMode.generated.h"

//Enum to store the current state of gameplay
UENUM(BlueprintType)
enum class EPrometheusPlayState : uint8
{
	EPlaying,
	EGameOver,
	EUnknown
};

UCLASS(minimalapi)
class APrometheusGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APrometheusGameMode();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Energy)
	float DecayRate;

	EPrometheusPlayState GetCurrentState() const;

	void SetCurrentState(EPrometheusPlayState NewState);

	virtual void BeginPlay() override;

private:
	TArray<ASpawnVolume*> SpawnVolumeActors;

	EPrometheusPlayState CurrentState;

	void HandleNewState(EPrometheusPlayState NewState);
};

FORCEINLINE EPrometheusPlayState APrometheusGameMode::GetCurrentState() const
{
	return CurrentState;
}



