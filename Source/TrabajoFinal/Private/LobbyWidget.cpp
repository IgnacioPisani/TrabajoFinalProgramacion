#include "LobbyWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "LobbyPlayerController.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	

	// Bindear botón listo
	if (btn_Ready)
		btn_Ready->OnClicked.AddDynamic(this, &ULobbyWidget::OnReadyClicked);

	if (txt_Status)
		txt_Status->SetText(FText::FromString(TEXT("Esperando jugadores...")));
}

void ULobbyWidget::UpdateReadyCount(int32 Ready, int32 Total)
{
	if (txt_Ready)
	{
		FString Texto = FString::Printf(TEXT("Listos: %d/%d"), Ready, Total);
		txt_Ready->SetText(FText::FromString(Texto));
	}
}

void ULobbyWidget::OnReadyClicked()
{
	if (bIsReady) return;  // no puede des-listarse
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

void ULobbyWidget::UpdatePlayerCount(int32 Current, int32 Required)
{
	if (txt_Count)
	{
		FString Texto = FString::Printf(TEXT("Jugadores: %d/%d"), Current, Required);
		txt_Count->SetText(FText::FromString(Texto));
	}

	if (txt_Status)
	{
		FString Status = Current >= Required
			? TEXT("Listo para iniciar!")
			: TEXT("Esperando jugadores...");
		txt_Status->SetText(FText::FromString(Status));
	}
}
