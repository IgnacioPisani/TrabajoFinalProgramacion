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
	// Se ejecuta en TODOS los clientes cuando GamePhase cambia
	// Acá después conectás con el widget para actualizar la UI
	UE_LOG(LogTemp, Warning, TEXT("GamePhase changed to: %d"), (int32)GamePhase);
}

void AZombieGameState::SetGamePhase(EGamePhase NewPhase)
{
	// Solo el servidor llama esta función
	GamePhase = NewPhase;
	OnRep_GamePhase(); // llamado manual en servidor (RepNotify no se auto-dispara en server)
}

void AZombieGameState::UpdateCounts(int32 Survivors, int32 Zombies)
{
	SurvivorCount = Survivors;
	ZombieCount = Zombies;
}