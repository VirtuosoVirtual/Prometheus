// Fill out your copyright notice in the Description page of Project Settings.

#include "Prometheus.h"
#include "CupcakePickup.h"
#include "CitizenAIController.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"

ACupcakePickup::ACupcakePickup()
{
	EnergyLevel = 250.0f;

	//Test Sight Sense Stimuli
	UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, UAISense_Sight::StaticClass(), this);
}

void ACupcakePickup::OnPickedUp_Implementation()
{
	Super::OnPickedUp_Implementation();

	Destroy();
}


