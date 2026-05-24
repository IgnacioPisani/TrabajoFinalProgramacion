#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

UCLASS()
class TRABAJOFINAL_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdatePlayerCount(int32 Current, int32 Required);
	void UpdateReadyCount(int32 Ready, int32 Total);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_Count = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_Status = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Ready = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_Ready = nullptr;
private:
	
	UFUNCTION()
	void OnReadyClicked();

	bool bIsReady = false;
};