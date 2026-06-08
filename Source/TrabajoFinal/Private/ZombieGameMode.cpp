// ZombieGameMode.cpp

#include "Public/ZombieGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Public/ZombieCharacter.h"
#include "Public/ZombieGameState.h"
#include "Public/ZombiePlayerController.h"
#include "Public/ZombiePlayerState.h"


AZombieGameMode::AZombieGameMode()
{
     PlayerStateClass = AZombiePlayerState::StaticClass();
     GameStateClass   = AZombieGameState::StaticClass();
     PlayerControllerClass = AZombiePlayerController::StaticClass();
     DefaultPawnClass = AZombieCharacter::StaticClass();
     bUseSeamlessTravel = true;

}

void AZombieGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    ConnectedPlayers.Add(NewPlayer);
    UE_LOG(LogTemp, Warning, TEXT("PostLogin - ConnectedPlayers: %d"), ConnectedPlayers.Num());
    UE_LOG(LogTemp, Warning, TEXT("HasAuthority: %s"), HasAuthority() ? TEXT("YES") : TEXT("NO"));

    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* Start = *It;
        if (!UsedSpawnPoints.Contains(Start))
        {
            UsedSpawnPoints.Add(Start);

            if (APawn* Pawn = NewPlayer->GetPawn())
            {
                FVector Loc = Start->GetActorLocation();
                FRotator Rot = Start->GetActorRotation();
                Pawn->SetActorLocationAndRotation(Loc, Rot);

                if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(NewPlayer))
                    ZPC->ClientForceRotation(Rot);
            }
            break;
        }
    }
    
    if (ConnectedPlayers.Num() >= MinPlayersToStart)
    {
        StartCountdown();
    }
}

void AZombieGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
    Super::HandleSeamlessTravelPlayer(C);

    if (APlayerController* PC = Cast<APlayerController>(C))
    {
        ConnectedPlayers.AddUnique(PC);
        UE_LOG(LogTemp, Warning, TEXT("HandleSeamlessTravelPlayer - Players: %d"), ConnectedPlayers.Num());

        // Asignar spawn point
        for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
        {
            APlayerStart* Start = *It;
            if (!UsedSpawnPoints.Contains(Start))
            {
                UsedSpawnPoints.Add(Start);

                if (APawn* Pawn = PC->GetPawn())
                {
                    FVector Loc = Start->GetActorLocation();
                    FRotator Rot = Start->GetActorRotation();
                    Pawn->SetActorLocationAndRotation(Loc, Rot);

                    if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
                        ZPC->ClientForceRotation(Rot);
                }
                break;
            }
        }

        // Arrancar cuando hay suficientes jugadores
        if (ConnectedPlayers.Num() >= MinPlayersToStart)
        {
            GetWorldTimerManager().ClearTimer(TimerHandle_StartGame);
            GetWorldTimerManager().SetTimer(
                TimerHandle_StartGame, this,
                &AZombieGameMode::StartCountdown,
                3.f, false);
        }
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

void AZombieGameMode::EndGame(bool bZombiesWon)
{
    GetWorldTimerManager().ClearTimer(TimerHandle_Countdown);

    if (AZombieGameState* GS = GetGameState<AZombieGameState>())
        GS->SetGamePhase(EGamePhase::GameOver);

    for (int32 i = 0; i < ConnectedPlayers.Num(); i++)
    {
        if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(ConnectedPlayers[i]))
        {
            // El host es siempre el índice 0
            bool bIsHost = (i == 0);
            ZPC->ClientGameOver(bZombiesWon, bIsHost); 
        }
    }
}

void AZombieGameMode::ReturnToLobby()
{
    // Resetear estado de todos los jugadores
    for (APlayerController* PC : ConnectedPlayers)
    {
        if (AZombiePlayerState* PS = PC->GetPlayerState<AZombiePlayerState>())
        {
            PS->SetIsZombie(false);
            PS->TeamID = FName("Survivor");
        }
    }
    ConnectedPlayers.Empty();
    UsedSpawnPoints.Empty();
    GetWorld()->ServerTravel("/Game/Maps/LobbyMap?listen");
}

void AZombieGameMode::StartInfection()
{
    if (ConnectedPlayers.Num() == 0) return;
    UE_LOG(LogTemp, Warning, TEXT("StartInfection - Players: %d"), ConnectedPlayers.Num());

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
            ZPC->ClientUpdateCount(ConnectedPlayers.Num() - 1, 1);
        }
    }

    if (AZombieGameState* GS = GetGameState<AZombieGameState>())
    {
        GS->SetGamePhase(EGamePhase::Playing);
        GS->TimeRemaining = GameDuration;
    }

    TWeakObjectPtr<AZombieGameMode> WeakThis(this);
    GetWorldTimerManager().SetTimer(
        TimerHandle_Countdown,
        [WeakThis]()
        {
            if (!WeakThis.IsValid()) return;

            AZombieGameState* GS = WeakThis->GetGameState<AZombieGameState>();
            if (!GS) return;

            GS->TimeRemaining -= 1.f;

            for (APlayerController* PC : WeakThis->ConnectedPlayers)
            {
                if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
                    ZPC->ClientUpdateTimer(GS->TimeRemaining);
            }

            if (GS->TimeRemaining <= 0.f)
            {
                WeakThis->GetWorldTimerManager().ClearTimer(WeakThis->TimerHandle_Countdown);
                WeakThis->EndGame(false);
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


void AZombieGameMode::StartCountdown()
{
    CountdownValue = 3;
    UE_LOG(LogTemp, Warning, TEXT("StartCountdown llamado - Players: %d"), ConnectedPlayers.Num());

    for (APlayerController* PC : ConnectedPlayers)
        if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
            ZPC->ClientShowCountdown(CountdownValue);

    TWeakObjectPtr<AZombieGameMode> WeakThis(this);
    GetWorldTimerManager().SetTimer(
        TimerHandle_Countdown_Pre,
        [WeakThis]()
        {
            if (!WeakThis.IsValid()) return;

            WeakThis->CountdownValue--;

            for (APlayerController* PC : WeakThis->ConnectedPlayers)
                if (AZombiePlayerController* ZPC = Cast<AZombiePlayerController>(PC))
                    ZPC->ClientShowCountdown(WeakThis->CountdownValue);

            if (WeakThis->CountdownValue <= 0)
            {
                WeakThis->GetWorldTimerManager().ClearTimer(WeakThis->TimerHandle_Countdown_Pre);
                WeakThis->StartInfection();
            }
        },
        1.f, true);
}