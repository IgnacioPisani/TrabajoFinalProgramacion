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
	DOREPLIFETIME(AZombiePlayerState, SelectedMaterialIndex);}

void AZombiePlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	UE_LOG(LogTemp, Warning, TEXT("CopyProperties llamado - SelectedMaterialIndex: %d"), SelectedMaterialIndex);

	if (AZombiePlayerState* ZPS = Cast<AZombiePlayerState>(PlayerState))
	{
		ZPS->SelectedMaterialIndex = SelectedMaterialIndex;
		UE_LOG(LogTemp, Warning, TEXT("CopyProperties copiado: %d"), ZPS->SelectedMaterialIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CopyProperties cast fallido"));
	}
}
void AZombiePlayerState::SetIsZombie(bool bNewValue)
{
	bIsZombie = bNewValue;
	TeamID = bNewValue ? FName("Zombie") : FName("Survivor");
	OnRep_IsZombie(); 
}

void AZombiePlayerState::SetSelectedMaterial(int32 Index)
{
	SelectedMaterialIndex = Index;
}

void AZombiePlayerState::OnRep_IsZombie()
{
    if (APawn* Pawn = GetPawn())
    {
        if (AZombieCharacter* ZC = Cast<AZombieCharacter>(Pawn))
            ZC->ApplyZombieVisuals(bIsZombie);
    }

	if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(GetOwner()))
		ZPC->ClientShowRole(bIsZombie);

}

void AZombiePlayerState::AddScore(float Amount)
{
	SetScore(GetScore() + Amount);
}