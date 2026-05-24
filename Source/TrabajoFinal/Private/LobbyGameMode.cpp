#include "LobbyGameMode.h"
#include "LobbyPlayerController.h"

ALobbyGameMode::ALobbyGameMode()
{
    bUseSeamlessTravel = true;  // viaje sin pantalla de carga abrupta
}
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    LobbyPlayers.Add(NewPlayer);

    // Delay para que el widget exista antes de actualizarlo
    FTimerHandle TimerHandle_UpdateCount;
    GetWorldTimerManager().SetTimer(
        TimerHandle_UpdateCount,
        [this]()
        {
            // Notificar a todos cuántos jugadores hay
            for (APlayerController* PC : LobbyPlayers)
            {
                if (ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(PC))
                    LPC->ClientUpdateLobbyCount(LobbyPlayers.Num(), MinPlayersToStart);
            }

            // Avisar al host si ya puede iniciar
            if (LobbyPlayers.Num() >= MinPlayersToStart)
            {
                if (ALobbyPlayerController* HostPC = Cast<ALobbyPlayerController>(LobbyPlayers[0]))
                {
                    HostPC->ClientUpdateLobbyCount(LobbyPlayers.Num(), MinPlayersToStart);
                }
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