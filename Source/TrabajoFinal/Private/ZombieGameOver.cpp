#include "Public/ZombieGameOver.h"

#include "ZombieGameMode.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

class AZombieGameMode;

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
	if (!bIsHost) return;

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetPause(false);

		if (UWorld* World = PC->GetWorld())
			World->ServerTravel(World->GetName() + "?listen");
	}
}

void UZombieGameOver::SetIsHost(bool bHost)
{
	bIsHost = bHost;

	if (btn_Volver)
	{
		btn_Volver->SetVisibility(bIsHost
			? ESlateVisibility::Visible
			: ESlateVisibility::Hidden);
	}
}

