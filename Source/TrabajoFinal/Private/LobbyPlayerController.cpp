#include "LobbyPlayerController.h"

#include "EngineUtils.h"
#include "LobbyGameMode.h"
#include "LobbyWidget.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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

