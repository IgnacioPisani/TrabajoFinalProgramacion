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


	UFUNCTION(Client, Unreliable)
	void ClientUpdateTimer(float TimeRemaining);

	UFUNCTION(Client, Reliable)
	void ClientUpdateCount(int32 Survivors, int32 Zombies);

	UFUNCTION(Client, Reliable)
	void ClientShowCountdown(int32 Number);
	
	UFUNCTION(Client, Reliable)
    void ClientForceRotation(FRotator NewRotation);

	UFUNCTION(Client, Reliable)
	void ClientGameOver(bool bZombiesWon, bool bIsHost);
	
protected:
	virtual void BeginPlay() override;

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
