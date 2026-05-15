// ZombieGameOver.cpp
#include "Public/ZombieGameOver.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UZombieGameOver::NativeConstruct()
{
	Super::NativeConstruct();

	if (btn_Volver)
		btn_Volver->OnClicked.AddDynamic(this, &UZombieGameOver::OnVolverClicked);
}

void UZombieGameOver::SetupResult(bool bZombiesWon, float Score)
{
	if (img_Resultado)
	{
		UTexture2D* Textura = bZombiesWon ? ImagenZombiesGanan : ImagenSobrevivientesGanan;
		if (Textura)
		{
			img_Resultado->SetBrushFromTexture(Textura);
		}
	}

	if (txt_Score)
	{
		FString ScoreTexto = FString::Printf(TEXT("Puntaje: %.0f"), Score);
		txt_Score->SetText(FText::FromString(ScoreTexto));
	}
}

void UZombieGameOver::OnVolverClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
		PC->SetPause(false);
	UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}