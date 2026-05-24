#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

UCLASS()
class TRABAJOFINAL_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	// Client RPCs
	UFUNCTION(Client, Reliable)
	void ClientUpdateLobbyCount(int32 Current, int32 Max, int32 Min);

	UFUNCTION(Client, Reliable)
	void ClientForceRotation(FRotator NewRotation);

	
	// Server RPC — jugador avisa que está listo
	UFUNCTION(Server, Reliable)
	void ServerSetReady();

	// Client RPC — actualizar conteo de listos
	UFUNCTION(Client, Reliable)
	void ClientUpdateReadyCount(int32 Ready, int32 Total);

	UFUNCTION(Server, Reliable)
	void ServerChangeColor(int32 ColorIndex);

	int32 SelectedMaterialIndex = 0;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class ULobbyWidget* LobbyHUD = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	bool bIsHost = false;
};