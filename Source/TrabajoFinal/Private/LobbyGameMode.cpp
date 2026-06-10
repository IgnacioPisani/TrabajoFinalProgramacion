#include "LobbyGameMode.h"
#include "LobbyPlayerController.h"
#include "Engine/TargetPoint.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ZombiePlayerState.h"
#include "GameFramework/Character.h"

ALobbyGameMode::ALobbyGameMode()
{
    bUseSeamlessTravel = true;  
    PlayerStateClass = AZombiePlayerState::StaticClass();  
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    LobbyPlayers.Add(NewPlayer);

    // Asignar slot
    int32 AssignedSlot = SlotOrder[FMath::Min(LobbyPlayers.Num() - 1, 3)];
    FTimerHandle TimerHandle_UpdateCount;
    GetWorldTimerManager().SetTimer(
        TimerHandle_UpdateCount,
        [this, NewPlayer, AssignedSlot]()
        {
            if (!NewPlayer) return;
            if (APawn* Pawn = NewPlayer->GetPawn())
            {
                // Ocultar antes de teleportar
                Pawn->SetActorHiddenInGame(true);

                FString TargetName = FString::Printf(TEXT("TP_Slot%d"), AssignedSlot);
                for (TActorIterator<ATargetPoint> It(GetWorld()); It; ++It)
                {
                    if (It->ActorHasTag(FName(*TargetName)))
                    {
                        Pawn->SetActorLocationAndRotation(
                            It->GetActorLocation(),
                            It->GetActorRotation()
                        );

                        if (ACharacter* Char = Cast<ACharacter>(Pawn))
                            Char->GetCharacterMovement()->DisableMovement();

                        FRotator TargetRot = It->GetActorRotation();
                        if (ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(NewPlayer))
                            LPC->ClientForceRotation(TargetRot);

                        // Mostrar después de un frame
                        FTimerHandle TimerHandle_Show;
                        GetWorldTimerManager().SetTimer(
                            TimerHandle_Show,
                            [Pawn]()
                            {
                                if (Pawn)
                                    Pawn->SetActorHiddenInGame(false);
                            },
                            0.2f, false);

                        break;
                    }
                }
            }

      // Notificar conteo a todos
      for (APlayerController* PC : LobbyPlayers)
      {
          if (ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(PC))
            LPC->ClientUpdateLobbyCount(LobbyPlayers.Num(), MaxPlayers, MinPlayersToStart);
      }
  },
  0.3f, false);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    if (APlayerController* PC = Cast<APlayerController>(Exiting))
    {
        LobbyPlayers.Remove(PC);
        ReadyPlayers.Remove(PC);
    }

    // Actualizar conteo a todos
    for (APlayerController* PC : LobbyPlayers)
    {
        if (ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(PC))
        {
            LPC->ClientUpdateLobbyCount(LobbyPlayers.Num(), MaxPlayers, MinPlayersToStart);
            LPC->ClientUpdateReadyCount(ReadyPlayers.Num(), LobbyPlayers.Num());
        }
    }
}

void ALobbyGameMode::SetPlayerReady(APlayerController* Player)
{
    if (!Player) return;
    if (ReadyPlayers.Contains(Player)) return;

    ReadyPlayers.Add(Player);

    // Notificar a todos cuántos están listos
    for (APlayerController* PC : LobbyPlayers)
    {
        if (ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(PC))
            LPC->ClientUpdateReadyCount(ReadyPlayers.Num(), LobbyPlayers.Num());
    }

    // Si todos están listos iniciar 
    if (ReadyPlayers.Num() >= LobbyPlayers.Num() && LobbyPlayers.Num() >= MinPlayersToStart)
        StartGame();
}

void ALobbyGameMode::StartGame()
{
    // Viajar al mapa de juego 
    FString TravelURL = GameMapPath + "?listen";
    GetWorld()->ServerTravel(TravelURL);
}