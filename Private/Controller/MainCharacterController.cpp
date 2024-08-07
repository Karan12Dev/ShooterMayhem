// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/MainCharacterController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MainCharacter/MainCharacter.h"



void AMainCharacterController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUDScreen->RemoveFromViewport();

	if (bIsWinner)
	{

		UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
		if (WinScreen)
		{
			WinScreen->AddToViewport();
			AMainCharacter* MainCharacter = Cast<AMainCharacter>(GetPawn());
			MainCharacter->Win();
		}
	}
	else
	{
		UUserWidget* LoseScreen = CreateWidget(this, LoseScreenClass);
		if (LoseScreen)
		{
			LoseScreen->AddToViewport();
		}
	}

	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}


void AMainCharacterController::BeginPlay()
{
	Super::BeginPlay();
	HUDScreen = CreateWidget(this, HUDClass);
	if (HUDScreen)
	{
		HUDScreen->AddToViewport();
	}
}
