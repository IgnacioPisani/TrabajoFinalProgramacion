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

	void StartGame();

	void SetPlayerReady(APlayerController* Player);

	TArray<APlayerController*> ReadyPlayers;

	TArray<FLobbyPlayerInfo> PlayerInfoList;

	TArray<FLobbyPlayerInfo> GetPlayerInfoList() { return PlayerInfoList; }

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 MinPlayersToStart = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	int32 MaxPlayers = 4;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FString GameMapPath = "/Game/Maps/Lvl_ThirdPerson";

	TMap<APlayerController*, int32> PlayerSlotMap;

	TArray<int32> SlotOrder = {1, 0, 2, 3};
	
	int32 NextSlotIndex = 0;
	
private:
	
	TArray<APlayerController*> LobbyPlayers;
	
};