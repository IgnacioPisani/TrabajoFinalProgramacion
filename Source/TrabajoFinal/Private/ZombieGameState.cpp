// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/ZombieGameState.h"
#include "Net/UnrealNetwork.h"

void AZombieGameState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AZombieGameState, TimeRemaining);
	DOREPLIFETIME(AZombieGameState, GamePhase);
	DOREPLIFETIME(AZombieGameState, SurvivorCount);
	DOREPLIFETIME(AZombieGameState, ZombieCount);
}

void AZombieGameState::SetGamePhase(EGamePhase NewPhase)
{
	GamePhase = NewPhase;
}

void AZombieGameState::UpdateCounts(int32 Survivors, int32 Zombies)
{
	SurvivorCount = Survivors;
	ZombieCount = Zombies;
}