// Fill out your copyright notice in the Description page of Project Settings.


#include "SesionPlayerController.h"

void ASesionPlayerController::BeginPlay()
{
	Super::BeginPlay();
	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());
}
