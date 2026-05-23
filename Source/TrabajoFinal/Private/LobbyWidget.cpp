#include "LobbyWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "LobbyPlayerController.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bindear botón
	if (btn_Start)
	{
		btn_Start->OnClicked.AddDynamic(this, &ULobbyWidget::OnStartClicked);
		btn_Start->SetIsEnabled(false);  // deshabilitado hasta que haya suficientes
		btn_Start->SetVisibility(ESlateVisibility::Hidden);  // oculto hasta que sea host
	}

	if (txt_Status)
		txt_Status->SetText(FText::FromString(TEXT("Esperando jugadores...")));
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

void ULobbyWidget::SetStartButtonEnabled(bool bEnabled)
{
	if (btn_Start)
		btn_Start->SetIsEnabled(bEnabled);
}

void ULobbyWidget::SetHostMode(bool bIsHost)
{
	if (btn_Start)
	{
		// Solo el host ve el botón
		btn_Start->SetVisibility(bIsHost
			? ESlateVisibility::Visible
			: ESlateVisibility::Hidden);
	}
}

void ULobbyWidget::OnStartClicked()
{
	if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
		PC->ServerRequestStartGame();
}