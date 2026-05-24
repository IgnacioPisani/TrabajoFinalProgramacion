#include "LobbyPlayerController.h"

#include "EngineUtils.h"
#include "LobbyGameMode.h"
#include "LobbyWidget.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"


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

	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());  // permite moverse Y clickear UI
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		SetViewTargetWithBlend(*It, 0.5f);
		break;
	}
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		if (It->GetActorLabel() == TEXT("LobbyCamera"))
		{
			SetViewTargetWithBlend(*It, 0.5f);
			break;
		}
	}
	bAutoManageActiveCameraTarget = false;
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

