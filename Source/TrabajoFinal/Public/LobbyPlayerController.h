#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

UCLASS()
class TRABAJOFINAL_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Server RPC — solo el host puede iniciar
	UFUNCTION(Server, Reliable)
	void ServerRequestStartGame();

	// Client RPCs
	UFUNCTION(Client, Reliable)
	void ClientUpdateLobbyCount(int32 Current, int32 Required);

	UFUNCTION(Client, Reliable)
	void ClientEnableStartButton(bool bEnabled);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class ULobbyWidget* LobbyHUD = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	bool bIsHost = false;
};