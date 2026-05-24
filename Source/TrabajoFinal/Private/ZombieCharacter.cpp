#include "Public/ZombieCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "ZombieGameInstance.h"
#include "Public/ZombieGameMode.h"
#include "Public/ZombiePlayerState.h"

class UZombieGameInstance;
class AZombieGameMode;
class AZombiePlayerState;

AZombieCharacter::AZombieCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    ZombieTrailFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ZombieTrailFX"));
    ZombieTrailFX->SetupAttachment(RootComponent);
    ZombieTrailFX->SetAutoActivate(false); 
}

void AZombieCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        FTimerHandle TimerHandle_Mat;
        GetWorldTimerManager().SetTimer(
            TimerHandle_Mat,
            [this]()
            {
                if (APlayerController* PC = Cast<APlayerController>(GetController()))
                {
                    if (UZombieGameInstance* GI = PC->GetGameInstance<UZombieGameInstance>())
                    {
                        FString PlayerName = PC->PlayerState 
                            ? PC->PlayerState->GetPlayerName() 
                            : TEXT("Unknown");
                        int32 Index = GI->GetPlayerMaterial(PlayerName);
                        UE_LOG(LogTemp, Warning, TEXT("BeginPlay - %s: material %d"), *PlayerName, Index);
                        AssignMaterial(Index);
                    }
                }
            },
            0.5f, false);
    }
}

void AZombieCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    AZombiePlayerState* PS = GetPlayerState<AZombiePlayerState>();
    UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerState - %s"), *GetName());
    UE_LOG(LogTemp, Warning, TEXT("PlayerState: %s"), PS ? TEXT("OK") : TEXT("NULL"));
    if (PS)
    {
        UE_LOG(LogTemp, Warning, TEXT("SelectedMaterialIndex: %d"), PS->SelectedMaterialIndex);
        UE_LOG(LogTemp, Warning, TEXT("PlayerMaterials num: %d"), PlayerMaterials.Num());
    }
}
void AZombieCharacter::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AZombieCharacter, Health);
}



void AZombieCharacter::ApplyZombieVisuals(bool bIsZombie)
{
    if (bIsZombie)
    {
        for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
            if (ZombieMaterial)
                GetMesh()->SetMaterial(i, ZombieMaterial);

        if (ZombieTrailFX)
            ZombieTrailFX->Activate(true);

        // Ragdoll
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
        GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("spine_01"), true, true);
        GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(
            this, &AZombieCharacter::OnCapsuleOverlap);
    }
    else
    {
        if (ZombieTrailFX)
            ZombieTrailFX->Deactivate();

        GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("spine_01"), false, true);
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(
            this, &AZombieCharacter::OnCapsuleOverlap);
    }
}

void AZombieCharacter::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (AZombieCharacter* OtherChar = Cast<AZombieCharacter>(OtherActor))
    {
        AZombiePlayerState* OtherPS = OtherChar->GetPlayerState<AZombiePlayerState>();
        if (OtherPS && !OtherPS->bIsZombie)
        {
            ServerInfectPlayer(OtherChar); 
        }
    }
}


void AZombieCharacter::ServerInfectPlayer_Implementation(AZombieCharacter* Victim)
{
    if (!Victim) return;

    AZombiePlayerState* VictimPS = Victim->GetPlayerState<AZombiePlayerState>();
    AZombiePlayerState* MyPS = GetPlayerState<AZombiePlayerState>();

    if (VictimPS && !VictimPS->bIsZombie && MyPS && MyPS->bIsZombie)
    {
        VictimPS->SetIsZombie(true); 

        Victim->NetMulticast_OnInfected(); 

        if (AZombieGameMode* GM = GetWorld()->GetAuthGameMode<AZombieGameMode>())
        {
            GM->CheckVictoryCondition();
        }
    }
}

void AZombieCharacter::AssignMaterial(int32 Index)
{
    if (!HasAuthority()) return;
    if (!PlayerMaterials.IsValidIndex(Index)) return;
    NetMulticast_SetMaterial(Index);
}



void AZombieCharacter::NetMulticast_SetMaterial_Implementation(int32 Index)
{
    if (!PlayerMaterials.IsValidIndex(Index)) return;

    for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
        GetMesh()->SetMaterial(i, PlayerMaterials[Index]);
}

void AZombieCharacter::NetMulticast_OnInfected_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("%s fue infectado!"), *GetName());

    // reproducir sonido 
}


void AZombieCharacter::TakeDamageZombie(float DamageAmount)
{
    if (!HasAuthority()) return; 
    Health -= DamageAmount;
    if (Health <= 0.f) OnDeath();
}

void AZombieCharacter::OnDeath()
{
    if (AZombiePlayerState* PS = GetPlayerState<AZombiePlayerState>())
    {
        if (!PS->bIsZombie)
        {
            PS->SetIsZombie(true);
            NetMulticast_OnInfected();

            if (AZombieGameMode* GM = GetWorld()->GetAuthGameMode<AZombieGameMode>())
            {
                GM->CheckVictoryCondition();
            }
        }
    }

    GetWorldTimerManager().SetTimer(
        TimerHandle_Respawn,
        [this]()
        {
            if (AZombieGameMode* GM = GetWorld()->GetAuthGameMode<AZombieGameMode>())
            {

            }
        },
        3.f, false
    );

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetVisibility(false);
}