// Fill out your copyright notice in the Description page of Project Settings.

#include "Prometheus.h"
#include "PrometheusHUD.h"
#include "PrometheusGameMode.h"
#include "PrometheusCharacter.h"
#include "CitizenAICharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"

APrometheusHUD::APrometheusHUD()
{
	//Use the RobotoDistanceField font from the engine
	static ConstructorHelpers::FObjectFinder<UFont>HUDFontOb(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	HUDFont = HUDFontOb.Object;
}

void APrometheusHUD::DrawHUD()
{
	//Get the screen dimensions
	FVector2D ScreenDimensions = FVector2D(Canvas->SizeX, Canvas->SizeY);

	//Call to the parent version of DrawHUD
	Super::DrawHUD();

	//Get the player character and print its power level
	APrometheusCharacter* MyCharacter = Cast<APrometheusCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	FString PowerLevelString = FString::Printf(TEXT("%10.1f"), FMath::Abs(MyCharacter->EnergyLevel));
	DrawText(PowerLevelString, FColor::White, 50, 50, HUDFont);

	APrometheusGameMode* MyGameMode = Cast<APrometheusGameMode>(UGameplayStatics::GetGameMode(this));

	//If the game is over
	if (MyGameMode->GetCurrentState() == EPrometheusPlayState::EGameOver)
	{
		//Create a variable for storing the size of printing Game Over
		FVector2D GameOverSize;
		GetTextSize(TEXT("GAME OVER"), GameOverSize.X, GameOverSize.Y, HUDFont);
		DrawText(TEXT("GAME OVER"), FColor::White, (ScreenDimensions.X - GameOverSize.X) / 2.0f, (ScreenDimensions.Y - GameOverSize.Y) / 2.0f, HUDFont);
	}
}

