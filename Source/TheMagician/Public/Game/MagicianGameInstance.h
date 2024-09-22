// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MagicianGameInstance.generated.h"

/**
 * This class is persisted across all Levels
 */
UCLASS()
class THEMAGICIAN_API UMagicianGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName PlayerStartTag = FName();

	UPROPERTY()
	FString LoadSlotName = FString();

	UPROPERTY()
	int32 LoadSlotIndex = 0;
};
