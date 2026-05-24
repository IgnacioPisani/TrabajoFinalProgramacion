#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"
USTRUCT()
struct FLobbyPlayerInfo
{
	GENERATED_BODY()

	FString PlayerName;
	bool bReady = false;
	APlayerController* Controller = nullptr;
};
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

	void SetPlayerReady(APlayerController* Player);

	TArray<APlayerController*> ReadyPlayers;

	TArray<FLobbyPlayerInfo> PlayerInfoList;

	// Llamado para obtener la lista actualizada
	TArray<FLobbyPlayerInfo> GetPlayerInfoList() { return PlayerInfoList; }
private:
	TArray<APlayerController*> LobbyPlayers;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 MinPlayersToStart = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FString GameMapPath = "/Game/Maps/Lvl_ThirdPerson";

	// Índice del slot asignado a cada controller
	TMap<APlayerController*, int32> PlayerSlotMap;

	// Orden de slots — el host siempre toma el índice 1 (centro-izquierda)
	TArray<int32> SlotOrder = {1, 0, 2, 3};
	int32 NextSlotIndex = 0;
	
};