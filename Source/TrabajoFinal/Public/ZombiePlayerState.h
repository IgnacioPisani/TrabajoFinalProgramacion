// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ZombiePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TRABAJOFINAL_API AZombiePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(ReplicatedUsing = OnRep_IsZombie, BlueprintReadOnly, Category = "Team")
	bool bIsZombie = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Team")
	FName TeamID = "Survivor";

	void AddScore(float Amount);
	void SetIsZombie(bool bNewValue);

	UFUNCTION()
	void OnRep_IsZombie();

protected:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
