// ZombieGameMode.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZombieGameMode.generated.h"

UCLASS()
class TRABAJOFINAL_API AZombieGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AZombieGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual void Logout(AController* Exiting) override;

	void StartInfection();
	void CheckVictoryCondition();
	void EndGame(bool bZombiesWon);
	void RespawnPlayer(AController* Controller);
	void StartCountdown();

private:
	TArray<APlayerController*> ConnectedPlayers;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 MinPlayersToStart = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	float GameDuration = 120.f;
	
	TArray<AActor*> UsedSpawnPoints;  
    
	FTimerHandle TimerHandle_StartGame;
	
	FTimerHandle TimerHandle_Countdown;

	int32 CountdownValue = 3;
	
	FTimerHandle TimerHandle_Countdown_Pre;  
};