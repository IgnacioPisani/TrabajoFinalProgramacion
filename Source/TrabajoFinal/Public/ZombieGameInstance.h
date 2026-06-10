#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ZombieGameInstance.generated.h"

UCLASS()
class TRABAJOFINAL_API UZombieGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	TMap<FString, int32> PlayerMaterialMap;

	void SetPlayerMaterial(FString PlayerName, int32 Index)
	{
		PlayerMaterialMap.Add(PlayerName, Index);
	}

	int32 GetPlayerMaterial(FString PlayerName)
	{
		if (int32* Index = PlayerMaterialMap.Find(PlayerName))
			return *Index;
		return 0;  // default
	}
};