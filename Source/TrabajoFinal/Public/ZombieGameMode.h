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
	
	void ReturnToLobby();
	
	virtual void Logout(AController* Exiting) override;
	
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	void StartInfection();
	
	void CheckVictoryCondition();
	
	void EndGame(bool bZombiesWon);
	
	void StartCountdown();

private:
	
	TArray<APlayerController*> ConnectedPlayers;

	TArray<AActor*> UsedSpawnPoints;  

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 MinPlayersToStart = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	float GameDuration = 120.f;
	
	FTimerHandle TimerHandle_StartGame;
	
	FTimerHandle TimerHandle_Countdown;

	FTimerHandle TimerHandle_Countdown_Pre;  

	int32 CountdownValue = 3;
	
};