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

void AZombieGameState::OnRep_GamePhase()
{
	UE_LOG(LogTemp, Warning, TEXT("GamePhase changed to: %d"), (int32)GamePhase);
}

void AZombieGameState::SetGamePhase(EGamePhase NewPhase)
{
	GamePhase = NewPhase;
	OnRep_GamePhase();
}

void AZombieGameState::UpdateCounts(int32 Survivors, int32 Zombies)
{
	SurvivorCount = Survivors;
	ZombieCount = Zombies;
}