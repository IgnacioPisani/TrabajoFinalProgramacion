// ZombieHUD.h
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZombieHUD.generated.h"

UCLASS()
class TRABAJOFINAL_API UZombieHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateTimer(float TimeRemaining);
	void UpdateRole(bool bIsZombie);
	void UpdateCount(int32 Survivors, int32 Zombies);
	void ShowRoleAlert(bool bIsZombie);
	void ShowInfectedOverlay();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_Tiempo = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_Rol = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_Conteo = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* img_Alerta = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* img_Rol = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* img_Count = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* img_Reloj = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UImage* img_InfectedOverlay = nullptr;  
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* ImagenZombiesInicial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* ImagenSobrevivientesInicial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* ImagenZombiesRol = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* ImagenSobrevivientesRol= nullptr;


private:
	FTimerHandle TimerHandle_HideAlert;
	FTimerHandle TimerHandle_HideOverlay;

	void FadeWidget(UWidget* Widget, bool bFadeIn, float Duration, TFunction<void()> OnComplete = nullptr);

	FTimerHandle TimerHandle_FadeAlert;
	FTimerHandle TimerHandle_FadeOverlay;

};