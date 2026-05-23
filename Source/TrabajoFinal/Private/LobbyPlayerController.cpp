#include "LobbyPlayerController.h"
#include "LobbyGameMode.h"
#include "LobbyWidget.h"
#include "Blueprint/UserWidget.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && LobbyWidgetClass)
	{
		LobbyHUD = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
		if (LobbyHUD) LobbyHUD->AddToViewport();
	}

	// El primer jugador es el host
	bIsHost = (GetWorld()->GetFirstPlayerController() == this);
}

void ALobbyPlayerController::ServerRequestStartGame_Implementation()
{
	// Solo el host puede iniciar — verificamos en el servidor
	if (GetWorld()->GetFirstPlayerController() == this)
	{
		if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
			GM->StartGame();
	}
}

void ALobbyPlayerController::ClientUpdateLobbyCount_Implementation(int32 Current, int32 Required)
{
	// Actualizar el widget con el conteo
	// Conectás con WBP_Lobby después
	UE_LOG(LogTemp, Warning, TEXT("Jugadores: %d/%d"), Current, Required);
}

void ALobbyPlayerController::ClientEnableStartButton_Implementation(bool bEnabled)
{
	// Habilitar o deshabilitar el botón de inicio en el widget
	UE_LOG(LogTemp, Warning, TEXT("Boton inicio: %s"), bEnabled ? TEXT("ON") : TEXT("OFF"));
}