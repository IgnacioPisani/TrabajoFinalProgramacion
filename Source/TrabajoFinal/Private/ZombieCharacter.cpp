#include "Public/ZombieCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Public/ZombieGameMode.h"
#include "Public/ZombiePlayerState.h"

class AZombieGameMode;
class AZombiePlayerState;

AZombieCharacter::AZombieCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    ZombieTrailFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ZombieTrailFX"));
    ZombieTrailFX->SetupAttachment(RootComponent);
    ZombieTrailFX->SetAutoActivate(false);  // empieza apagado
}

void AZombieCharacter::BeginPlay()
{
    Super::BeginPlay();
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
        // Material
        for (int32 i = 0; i < GetMesh()->GetNumMaterials(); i++)
            if (ZombieMaterial)
                GetMesh()->SetMaterial(i, ZombieMaterial);

        // Activar estela
        if (ZombieTrailFX)
            ZombieTrailFX->Activate(true);

        // Ragdoll y overlap — código existente
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
        GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("spine_01"), true, true);
        GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(
            this, &AZombieCharacter::OnCapsuleOverlap);
    }
    else
    {
        // Desactivar estela si vuelve a ser survivor
        if (ZombieTrailFX)
            ZombieTrailFX->Deactivate();

        // Resto del código existente
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