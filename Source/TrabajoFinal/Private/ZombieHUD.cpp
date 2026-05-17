// ZombieHUD.cpp
#include "Public/ZombieHUD.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/Image.h"

void UZombieHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if (img_Alerta)
		img_Alerta->SetVisibility(ESlateVisibility::Hidden);
	if (img_Rol)
		img_Rol->SetVisibility(ESlateVisibility::Hidden);
	if (img_Reloj)
		img_Reloj->SetVisibility(ESlateVisibility::Hidden);
	if (img_Count)
		img_Count->SetVisibility(ESlateVisibility::Hidden);
	if (img_InfectedOverlay)
		img_InfectedOverlay->SetVisibility(ESlateVisibility::Hidden);
	if (txt_Rol)
		txt_Rol->SetVisibility(ESlateVisibility::Hidden);
	if (txt_Conteo)
		txt_Conteo->SetVisibility(ESlateVisibility::Hidden);
	if (txt_Tiempo)
		txt_Tiempo->SetVisibility(ESlateVisibility::Hidden);
	if (txt_Countdown)
		txt_Countdown->SetVisibility(ESlateVisibility::Hidden);
}

void UZombieHUD::FadeWidget(UWidget* Widget, bool bFadeIn, float Duration, TFunction<void()> OnComplete)
{
	if (!Widget) return;

	float StartAlpha = bFadeIn ? 0.f : 1.f;
	float EndAlpha   = bFadeIn ? 1.f : 0.f;

	Widget->SetRenderOpacity(StartAlpha);
	Widget->SetVisibility(ESlateVisibility::Visible);

	int32 Steps = 30;
	float StepTime = Duration / Steps;
	float AlphaStep = (EndAlpha - StartAlpha) / Steps;

	TSharedPtr<int32> CurrentStep = MakeShared<int32>(0);
	TSharedPtr<FTimerHandle> Handle = MakeShared<FTimerHandle>();

	GetWorld()->GetTimerManager().SetTimer(
		*Handle,
		[this, Widget, CurrentStep, Steps, StartAlpha, AlphaStep, EndAlpha, Handle, OnComplete, bFadeIn]()
		{
			(*CurrentStep)++;
			float NewAlpha = StartAlpha + AlphaStep * (*CurrentStep);
			NewAlpha = FMath::Clamp(NewAlpha, 0.f, 1.f);
			Widget->SetRenderOpacity(NewAlpha);

			if (*CurrentStep >= Steps)
			{
				GetWorld()->GetTimerManager().ClearTimer(*Handle);
				Widget->SetRenderOpacity(EndAlpha);

				if (!bFadeIn)
					Widget->SetVisibility(ESlateVisibility::Hidden);

				if (OnComplete)
					OnComplete();
			}
		},
		StepTime, true);
}

void UZombieHUD::UpdateTimer(float TimeRemaining)
{
	if (!txt_Tiempo) return;

	int32 Minutos = FMath::FloorToInt(TimeRemaining / 60.f);
	int32 Segundos = FMath::FloorToInt(FMath::Fmod(TimeRemaining, 60.f));
	FString Texto = FString::Printf(TEXT("%02d:%02d"), Minutos, Segundos);
	txt_Tiempo->SetText(FText::FromString(Texto));
	txt_Tiempo->SetVisibility(ESlateVisibility::Visible);
	img_Reloj->SetVisibility(ESlateVisibility::Visible);
}

void UZombieHUD::UpdateRole(bool bIsZombie)
{
	if (!txt_Rol) return;

	if (img_Rol)
	{
		UTexture2D* Textura = bIsZombie ? ImagenZombiesRol : ImagenSobrevivientesRol;
		if (Textura)
		{
			img_Rol->SetBrushFromTexture(Textura);
			img_Rol->SetVisibility(ESlateVisibility::Visible);

		}
	}
	FString Rol = bIsZombie ? TEXT("INFECTA!") : TEXT("SOBREVIVE!");
	txt_Rol->SetText(FText::FromString(Rol));
	txt_Rol->SetVisibility(ESlateVisibility::Visible);
}

void UZombieHUD::UpdateCount(int32 Survivors, int32 Zombies)
{
	if (!txt_Conteo) return;

	FString Texto = FString::Printf(
		TEXT("%d/%d"), Zombies, Survivors+Zombies);
	txt_Conteo->SetText(FText::FromString(Texto));
	txt_Conteo->SetVisibility(ESlateVisibility::Visible);
	img_Count->SetVisibility(ESlateVisibility::Visible);
}

void UZombieHUD::ShowRoleAlert(bool bIsZombie)
{
	if (!img_Alerta) return;

	UTexture2D* Textura = bIsZombie ? ImagenZombiesInicial : ImagenSobrevivientesInicial;
	if (Textura)
		img_Alerta->SetBrushFromTexture(Textura);

	FadeWidget(img_Alerta, true, 0.5f, [this]()
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_HideAlert,
			[this]() { FadeWidget(img_Alerta, false, 0.5f); },
			2.f, false);
	});
}

void UZombieHUD::ShowInfectedOverlay()
{
	if (!img_InfectedOverlay) return;

	FadeWidget(img_InfectedOverlay, true, 0.3f, [this]()
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_HideOverlay,
			[this]() { FadeWidget(img_InfectedOverlay, false, 0.5f); },
			2.f, false);
	});
}

void UZombieHUD::ShowCountdown(int32 Number)
{
	if (!txt_Countdown) return;

	// Si es 0 ocultás el texto
	if (Number <= 0)
	{
		FadeWidget(txt_Countdown, false, 0.3f);
		return;
	}

	FString Texto = FString::Printf(TEXT("%d"), Number);
	txt_Countdown->SetText(FText::FromString(Texto));
	txt_Countdown->SetVisibility(ESlateVisibility::Visible);

	// Fade in rápido y fade out antes del siguiente número
	FadeWidget(txt_Countdown, true, 0.2f);
}