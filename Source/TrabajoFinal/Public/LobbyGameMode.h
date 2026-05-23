#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

UCLASS()
class TRABAJOFINAL_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	// El host llama esto para iniciar la partida
	void StartGame();

private:
	TArray<APlayerController*> LobbyPlayers;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 MinPlayersToStart = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FString GameMapPath = "/Game/Maps/GameMap";
};