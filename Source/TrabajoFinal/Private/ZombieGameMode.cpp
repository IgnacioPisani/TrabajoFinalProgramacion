// ZombieGameMode.cpp

#include "Public/ZombieGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Public/ZombieCharacter.h"
#include "Public/ZombieGameState.h"
#include "Public/ZombiePlayerController.h"
#include "Public/ZombiePlayerState.h"

class AZombiePlayerController;
class AZombiePlayerState;
class AZombieGameState;

AZombieGameMode::AZombieGameMode()
{
     PlayerStateClass = AZombiePlayerState::StaticClass();
     GameStateClass   = AZombieGameState::StaticClass();
     PlayerControllerClass = AZombiePlayerController::StaticClass();
     DefaultPawnClass = AZombieCharacter::StaticClass();
}

void AZombieGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    ConnectedPlayers.Add(NewPlayer);

    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* Start = *It;
        if (!UsedSpawnPoints.Contains(Start))
        {
            UsedSpawnPoints.Add(Start);

            if (APawn* Pawn = NewPlayer->GetPawn())
            {
                Pawn->SetActorLocationAndRotation(
                    Start->GetActorLocation(),
                    Start->GetActorRotation()
                );
            }
            break;
        }
    }
    
    if (ConnectedPlayers.Num() >= MinPlayersToStart)
    {
        StartCountdown();
    }
}

void AZombieGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    if (APlayerController* PC = Cast<APlayerController>(Exiting))
        ConnectedPlayers.Remove(PC);
}

void AZombieGameMode::BeginPlay()
{
    Super::BeginPlay();
    if (AZombieGameState* GS = GetGameState<AZombieGameState>())
        GS->SetGamePhase(EGamePhase::Waiting);
}

void AZombieGameMode::StartInfection()
{
    if (ConnectedPlayers.Num() == 0) return;

    int32 Idx = FMath::RandRange(0, ConnectedPlayers.Num() - 1);
    APlayerController* ChosenPC = ConnectedPlayers[Idx];

    if (AZombiePlayerState* PS = ChosenPC->GetPlayerState<AZombiePlayerState>())
        PS->SetIsZombie(true);

    for (APlayerController* PC : ConnectedPlayers)
    {
        if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
        {
            bool bIsZombie = (PC == ChosenPC);
            ZPC->ClientShowRole(bIsZombie);
        }
    }
    for (APlayerController* PC : ConnectedPlayers)
    {
        if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
            ZPC->ClientUpdateCount(ConnectedPlayers.Num() - 1, 1);
    }

    if (AZombieGameState* GS = GetGameState<AZombieGameState>())
    {
        GS->SetGamePhase(EGamePhase::Playing);
        GS->TimeRemaining = GameDuration;
    }

    GetWorldTimerManager().SetTimer(
        TimerHandle_Countdown,
        [this]()
        {
            AZombieGameState* GS = GetGameState<AZombieGameState>();
            if (!GS) return;
            GS->TimeRemaining -= 1.f;

      for (APlayerController* PC : ConnectedPlayers)
      {
          if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
              ZPC->ClientUpdateTimer(GS->TimeRemaining);
      }

      if (GS->TimeRemaining <= 0.f)
      {
          GetWorldTimerManager().ClearTimer(TimerHandle_Countdown);
          EndGame(false);
      }
        },
        1.f, true); 
}

void AZombieGameMode::CheckVictoryCondition()
{
    int32 Survivors = 0;
    for (APlayerController* PC : ConnectedPlayers)
    {
        if (AZombiePlayerState* PS = PC->GetPlayerState<AZombiePlayerState>())
            if (!PS->bIsZombie) Survivors++;
    }

    int32 Zombies = ConnectedPlayers.Num() - Survivors;

    if (AZombieGameState* GS = GetGameState<AZombieGameState>())
        GS->UpdateCounts(Survivors, Zombies);

    for (APlayerController* PC : ConnectedPlayers)
    {
        if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
            ZPC->ClientUpdateCount(Survivors, Zombies);
    }

    if (Survivors == 0)
        EndGame(true);
}

void AZombieGameMode::EndGame(bool bZombiesWon)
{
    GetWorldTimerManager().ClearTimer(TimerHandle_Countdown);
    UsedSpawnPoints.Empty(); 
    if (AZombieGameState* GS = GetGameState<AZombieGameState>())
        GS->SetGamePhase(EGamePhase::GameOver);

    for (APlayerController* PC : ConnectedPlayers)
    {
        if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
            ZPC->ClientGameOver(bZombiesWon);
    }
}

void AZombieGameMode::RespawnPlayer(AController* Controller)
{
    if (!Controller) return;

    AActor* StartSpot = FindPlayerStart(Controller);
    FVector SpawnLoc = StartSpot ? StartSpot->GetActorLocation() : FVector::ZeroVector;

    APawn* NewPawn = SpawnDefaultPawnFor(Controller, StartSpot);
    Controller->Possess(NewPawn);
}

void AZombieGameMode::StartCountdown()
{
    CountdownValue = 3;

    // Notificar a todos el número inicial
    for (APlayerController* PC : ConnectedPlayers)
        if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
            ZPC->ClientShowCountdown(CountdownValue);

    GetWorldTimerManager().SetTimer(
        TimerHandle_Countdown_Pre,
        [this]()
        {
            CountdownValue--;

            // Enviar número a todos los clientes
            for (APlayerController* PC : ConnectedPlayers)
                if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
                    ZPC->ClientShowCountdown(CountdownValue);

            // Cuando llega a 0 arranca el juego
            if (CountdownValue <= 0)
            {
                GetWorldTimerManager().ClearTimer(TimerHandle_Countdown_Pre);
                StartInfection();
            }
        },
        1.f, true);  // cada 1 segundo
}