// ZombieGameOver.h
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ZombieGameOver.generated.h"

UCLASS()
class TRABAJOFINAL_API UZombieGameOver : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupResult(bool bZombiesWon, float Score);

	void SetIsHost(bool bHost);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UImage* img_Resultado = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* ImagenZombiesGanan = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* ImagenSobrevivientesGanan = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_Score = nullptr;

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Volver = nullptr;
	
	bool bIsHost = false;

private:
	UFUNCTION()
	void OnVolverClicked();
};