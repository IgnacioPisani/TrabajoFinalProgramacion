// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrabajoFinalPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "ZombiePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TRABAJOFINAL_API AZombiePlayerController : public ATrabajoFinalPlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void ClientShowRole(bool bIsZombie);

	UFUNCTION(Client, Reliable)
	void ClientGameOver(bool bZombiesWon);

	UFUNCTION(Client, Unreliable)
	void ClientUpdateTimer(float TimeRemaining);

	UFUNCTION(Client, Reliable)
	void ClientUpdateCount(int32 Survivors, int32 Zombies);
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:

	UPROPERTY()
	class UZombieHUD* HUDWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UZombieHUD> HUDWidgetClass;

	UPROPERTY()
	class UZombieGameOver* GameOverWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UZombieGameOver> GameOverWidgetClass;
};
