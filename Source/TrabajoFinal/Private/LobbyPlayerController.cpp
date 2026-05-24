#include "LobbyPlayerController.h"

#include "EngineUtils.h"
#include "LobbyGameMode.h"
#include "LobbyWidget.h"
#include "ZombieCharacter.h"
#include "ZombieGameInstance.h"
#include "ZombiePlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

class UZombieGameInstance;
class AZombiePlayerState;

void ALobbyPlayerController::ClientForceRotation_Implementation(FRotator NewRotation)
{
	SetControlRotation(NewRotation);

	// También rotar el Pawn directamente
	if (APawn* MyPawn  = GetPawn())
	{
		MyPawn ->SetActorRotation(NewRotation);

		// Bloquear el SpringArm para que no sobreescriba
		if (ACharacter* Char = Cast<ACharacter>(MyPawn))
		{
			Char->GetCharacterMovement()->DisableMovement();
			Char->bUseControllerRotationYaw = true;
		}
	}

	FTimerHandle TimerHandle_Rot;
	GetWorldTimerManager().SetTimer(
		TimerHandle_Rot,
		[this, NewRotation]()
		{
			SetControlRotation(NewRotation);
			if (APawn* Pawn = GetPawn())
				Pawn->SetActorRotation(NewRotation);
		},
		0.5f, false);
}

void ALobbyPlayerController::ClientUpdateLobbyCount_Implementation(int32 Current, int32 Max, int32 Min)
{
	if (LobbyHUD)
		LobbyHUD->UpdatePlayerCount(Current, Max, Min);
}

void ALobbyPlayerController::ServerChangeColor_Implementation(int32 ColorIndex)
{
	if (UZombieGameInstance* GI = GetGameInstance<UZombieGameInstance>())
	{
		FString PlayerName = PlayerState ? PlayerState->GetPlayerName() : TEXT("Unknown");
		GI->SetPlayerMaterial(PlayerName, ColorIndex);
		UE_LOG(LogTemp, Warning, TEXT("GI guardado - %s: %d"), *PlayerName, ColorIndex);
	}

	if (AZombiePlayerState* PS = GetPlayerState<AZombiePlayerState>())
		PS->SetSelectedMaterial(ColorIndex);

	if (APawn* MyPawn = GetPawn())
		if (AZombieCharacter* ZC = Cast<AZombieCharacter>(MyPawn))
			ZC->AssignMaterial(ColorIndex);
}

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
			int32 Required = 4;
			if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
				Required = GM->MaxPlayers;  

			LobbyHUD->UpdatePlayerCount(1, Required);
		}
	}

	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());
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


