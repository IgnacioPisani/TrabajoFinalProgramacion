#include "LobbyGameMode.h"
#include "LobbyPlayerController.h"

ALobbyGameMode::ALobbyGameMode()
{
    bUseSeamlessTravel = true;  // viaje sin pantalla de carga abrupta
}
#include "Engine/TargetPoint.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    LobbyPlayers.Add(NewPlayer);

    // Asignar slot — host toma el centroint32 SlotOrder[] = {1, 0, 2, 3};
    SlotOrder = {1, 0, 2, 3};
    int32 AssignedSlot = SlotOrder[FMath::Min(LobbyPlayers.Num() - 1, 3)];
    FTimerHandle TimerHandle_UpdateCount;
    GetWorldTimerManager().SetTimer(
        TimerHandle_UpdateCount,
        [this, NewPlayer, AssignedSlot]()
        {
            // Teleportar al Target Point correspondiente
            if (APawn* Pawn = NewPlayer->GetPawn())
            {
                // Buscar el TP por nombre
                FString TargetName = FString::Printf(TEXT("TP_Slot%d"), AssignedSlot);
                for (TActorIterator<ATargetPoint> It(GetWorld()); It; ++It)
                {
                     if (It->GetActorLabel().Contains(TargetName))  
                    {
                        Pawn->SetActorLocationAndRotation(
                            It->GetActorLocation(),
                            It->GetActorRotation()
                        );
                        NewPlayer->SetControlRotation(It->GetActorRotation());
                    }
                    UE_LOG(LogTemp, Warning, TEXT("TargetPoint encontrado: %s"), *It->GetActorLabel());
                }
            }

            // Notificar conteo a todos
            for (APlayerController* PC : LobbyPlayers)
            {
                if (ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(PC))
                    LPC->ClientUpdateLobbyCount(LobbyPlayers.Num(), MinPlayersToStart);
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
            LPC->ClientUpdateLobbyCount(LobbyPlayers.Num(), MinPlayersToStart);
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

    // Si todos están listos → iniciar directamente, sin botón
    if (ReadyPlayers.Num() >= LobbyPlayers.Num() && LobbyPlayers.Num() >= MinPlayersToStart)
        StartGame();  // <- directo, sin pasar por el host
}

void ALobbyGameMode::StartGame()
{
    // Viajar al mapa de juego — arrastra a todos los clientes
    FString TravelURL = GameMapPath + "?listen";
    GetWorld()->ServerTravel(TravelURL);
}