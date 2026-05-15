// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/ZombiePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "Public/ZombieGameOver.h"
#include "Public/ZombieHUD.h"

void AZombiePlayerController::ClientUpdateCount_Implementation(int32 Survivors, int32 Zombies)
{
	if (HUDWidget)
		HUDWidget->UpdateCount(Survivors, Zombies);
}

void AZombiePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UZombieHUD>(this, HUDWidgetClass);
		if (HUDWidget) HUDWidget->AddToViewport();
	}
}

void AZombiePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AZombiePlayerController::ClientShowRole_Implementation(bool bIsZombie)
{
	if (HUDWidget)
	{
		HUDWidget->UpdateRole(bIsZombie);
		HUDWidget->ShowRoleAlert(bIsZombie);
		if (bIsZombie)
			HUDWidget->ShowInfectedOverlay();
	}
}

void AZombiePlayerController::ClientUpdateTimer_Implementation(float TimeRemaining)
{
	if (HUDWidget)
		HUDWidget->UpdateTimer(TimeRemaining);
}



void AZombiePlayerController::ClientGameOver_Implementation(bool bZombiesWon)
{
	SetPause(true);

	if (HUDWidget)
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);

	if (GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<UZombieGameOver>(this, GameOverWidgetClass);
		if (GameOverWidget)
		{
			float Score = 0.f;
			if (APlayerState* PS = PlayerState)
				Score = PS->GetScore();

			GameOverWidget->SetupResult(bZombiesWon, Score);
			GameOverWidget->AddToViewport();
		}
	}

	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;
}