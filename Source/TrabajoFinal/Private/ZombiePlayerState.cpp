// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/ZombiePlayerState.h"

#include "ZombiePlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Public/ZombieCharacter.h"

class AZombieCharacter;

void AZombiePlayerState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AZombiePlayerState, bIsZombie);
	DOREPLIFETIME(AZombiePlayerState, TeamID);
}

void AZombiePlayerState::SetIsZombie(bool bNewValue)
{
	bIsZombie = bNewValue;
	TeamID = bNewValue ? FName("Zombie") : FName("Survivor");
	OnRep_IsZombie(); 
}

void AZombiePlayerState::OnRep_IsZombie()
{
    if (APawn* Pawn = GetPawn())
    {
        if (AZombieCharacter* ZC = Cast<AZombieCharacter>(Pawn))
            ZC->ApplyZombieVisuals(bIsZombie);
    }

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
	{
		ZPC->ClientShowRole(bIsZombie);
	}

    UE_LOG(LogTemp, Warning, TEXT("Player %s is now: %s"),
        *GetPlayerName(), bIsZombie ? TEXT("ZOMBIE") : TEXT("SURVIVOR"));
}

void AZombiePlayerState::AddScore(float Amount)
{
	SetScore(GetScore() + Amount);
}