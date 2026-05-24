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
		if (LobbyHUD)
		{
			LobbyHUD->AddToViewport();
		    bIsHost = (GetWorld()->GetFirstPlayerController() == this);
			LobbyHUD->UpdatePlayerCount(1, 2);
		}
	}

	// Habilitar cursor y modo UI+juego para poder clickear y moverse
	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());  // permite moverse Y clickear UI
}

void ALobbyPlayerController::ServerSetReady_Implementation()
{
	if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
		GM->SetPlayerReady(this);
}

void ALobbyPlayerController::ClientUpdateReadyCount_Implementation(int32 Ready, int32 Total)
{
	if (LobbyHUD)
		LobbyHUD->UpdateReadyCount(Ready, Total);
}


void ALobbyPlayerController::ClientUpdateLobbyCount_Implementation(int32 Current, int32 Required)
{
	UE_LOG(LogTemp, Warning, TEXT("ClientUpdateLobbyCount: %d/%d"), Current, Required);
	if (LobbyHUD)
		LobbyHUD->UpdatePlayerCount(Current, Required);
	else
		UE_LOG(LogTemp, Warning, TEXT("LobbyHUD es NULL"));
}

