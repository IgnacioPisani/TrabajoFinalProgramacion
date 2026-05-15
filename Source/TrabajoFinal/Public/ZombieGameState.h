// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ZombieGameState.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Waiting,
	Playing,
	GameOver
};

/**
 * 
 */
UCLASS()
class TRABAJOFINAL_API AZombieGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
	float TimeRemaining = 120.f;

	UPROPERTY(ReplicatedUsing = OnRep_GamePhase, BlueprintReadOnly, Category = "Game")
	EGamePhase GamePhase = EGamePhase::Waiting;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
	int32 SurvivorCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game")
	int32 ZombieCount = 0;

	UFUNCTION()
	void OnRep_GamePhase();

	void SetGamePhase(EGamePhase NewPhase);
	void UpdateCounts(int32 Survivors, int32 Zombies);

protected:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
