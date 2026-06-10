#include "LobbyWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "LobbyPlayerController.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (btn_Color0)
		btn_Color0->OnClicked.AddDynamic(this, &ULobbyWidget::OnColor0Clicked);
	if (btn_Color1)
		btn_Color1->OnClicked.AddDynamic(this, &ULobbyWidget::OnColor1Clicked);
	if (btn_Color2)
		btn_Color2->OnClicked.AddDynamic(this, &ULobbyWidget::OnColor2Clicked);
	if (btn_Color3)
		btn_Color3->OnClicked.AddDynamic(this, &ULobbyWidget::OnColor3Clicked);
	if (btn_Ready)
		btn_Ready->OnClicked.AddDynamic(this, &ULobbyWidget::OnReadyClicked);

	if (txt_Status)
		txt_Status->SetText(FText::FromString(TEXT("Esperando jugadores...")));

	if (txt_Ready)
		txt_Ready->SetVisibility(ESlateVisibility::Hidden);

	if (btn_Ready)
		btn_Ready->SetVisibility(ESlateVisibility::Hidden);
}

void ULobbyWidget::UpdatePlayerCount(int32 Current, int32 Max, int32 Min)
{
	if (txt_Count)
	{
		FString Texto = FString::Printf(TEXT("JUGADORES: %d/%d"), Current, Max);
		txt_Count->SetText(FText::FromString(Texto));
	}

	if (txt_Status)
	{
		FString Status = Current >= Min
			? TEXT("LISTO PARA INICIAR!")
			: TEXT("ESPERANDO...");
		txt_Status->SetText(FText::FromString(Status));
	}
	if (Current>=Min)
	{
		if (btn_Ready)
		{
			btn_Ready->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ULobbyWidget::UpdateReadyCount(int32 Ready, int32 Total)
{
	if (txt_Ready)
	{
		if (Ready > 0)
		{
			txt_Ready->SetVisibility(ESlateVisibility::Visible);
			FString Texto = FString::Printf(TEXT("Listos: %d/%d"), Ready, Total);
			txt_Ready->SetText(FText::FromString(Texto));
		}
	}
}

void ULobbyWidget::OnReadyClicked()
{
	if (bIsReady) return;  
	bIsReady = true;

	// Cambiar visual del botón para mostrar que está listo
	if (btn_Ready)
	{
		btn_Ready->SetIsEnabled(false);
		if (UTextBlock* BtnText = Cast<UTextBlock>(btn_Ready->GetChildAt(0)))
			BtnText->SetText(FText::FromString(TEXT("LISTO!")));
	}

	if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
		PC->ServerSetReady();
}


void ULobbyWidget::OnColor0Clicked() { RequestColorChange(0); }
void ULobbyWidget::OnColor1Clicked() { RequestColorChange(1); }
void ULobbyWidget::OnColor2Clicked() { RequestColorChange(2); }
void ULobbyWidget::OnColor3Clicked() { RequestColorChange(3); }

void ULobbyWidget::RequestColorChange(int32 ColorIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("RequestColorChange: %d"), ColorIndex);
	if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
		PC->ServerChangeColor(ColorIndex);
	else
		UE_LOG(LogTemp, Warning, TEXT("PlayerController NULL en RequestColorChange"));
}