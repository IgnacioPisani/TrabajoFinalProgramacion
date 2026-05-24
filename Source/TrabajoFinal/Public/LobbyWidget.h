#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

UCLASS()
class TRABAJOFINAL_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
void UpdatePlayerCount(int32 Current, int32 Max, int32 Min = 2);
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

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Color0 = nullptr;  

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Color1 = nullptr;  

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Color2 = nullptr;  

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Color3 = nullptr;  
private:
	UFUNCTION()
	void OnColor0Clicked();
	UFUNCTION()
	void OnColor1Clicked();
	UFUNCTION()
	void OnColor2Clicked();
	UFUNCTION()
	void OnColor3Clicked();

	void RequestColorChange(int32 ColorIndex);
	
	UFUNCTION()
	void OnReadyClicked();

	bool bIsReady = false;
};