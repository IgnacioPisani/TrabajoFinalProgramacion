#pragma once
#include "CoreMinimal.h"
#include "TrabajoFinalCharacter.h"
#include "GameFramework/Character.h"
#include "ZombieCharacter.generated.h"

UCLASS()
class TRABAJOFINAL_API AZombieCharacter : public ATrabajoFinalCharacter
{
	GENERATED_BODY()

public:
	AZombieCharacter();

	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
	float Health = 100.f;

	void ApplyZombieVisuals(bool bIsZombie);

	UFUNCTION()
	void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void ServerInfectPlayer(AZombieCharacter* Victim);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnInfected();

	void TakeDamageZombie(float DamageAmount);
	void OnDeath();

	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	TArray<UMaterialInterface*> PlayerMaterials;

	void AssignMaterial(int32 Index);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SetMaterial(int32 Index);
protected:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	UMaterialInterface* ZombieMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Visuals")
	UMaterialInterface* SurvivorMaterial = nullptr;

	
private:
	FTimerHandle TimerHandle_Respawn;

	UPROPERTY(VisibleAnywhere, Category = "FX")
	class UNiagaraComponent* ZombieTrailFX = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class UNiagaraSystem* ZombieTrailSystem = nullptr;
};
    

