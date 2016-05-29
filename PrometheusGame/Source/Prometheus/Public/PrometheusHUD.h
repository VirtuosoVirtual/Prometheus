// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "PrometheusHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROMETHEUS_API APrometheusHUD : public AHUD
{
	GENERATED_BODY()
	APrometheusHUD();

	//Variable for storing the font
	UPROPERTY()
	UFont* HUDFont;

	//Primary draw call for the HUD
	virtual void DrawHUD() override;
};
