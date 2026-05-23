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
            HostPC->ClientEnableStartButton(true);
    }
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    if (APlayerController* PC = Cast<APlayerController>(Exiting))
        LobbyPlayers.Remove(PC);

    // Actualizar conteo
    for (APlayerController* PC : LobbyPlayers)
    {
        if (ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(PC))
            LPC->ClientUpdateLobbyCount(LobbyPlayers.Num(), MinPlayersToStart);
    }

    // Deshabilitar botón si ya no hay suficientes
    if (LobbyPlayers.Num() < MinPlayersToStart)
    {
        if (ALobbyPlayerController* HostPC = Cast<ALobbyPlayerController>(LobbyPlayers[0]))
            HostPC->ClientEnableStartButton(false);
    }
}

void ALobbyGameMode::StartGame()
{
    // Viajar al mapa de juego — arrastra a todos los clientes
    FString TravelURL = GameMapPath + "?listen";
    GetWorld()->ServerTravel(TravelURL);
}