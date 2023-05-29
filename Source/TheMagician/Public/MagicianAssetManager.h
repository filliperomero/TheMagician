// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MagicianAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class THEMAGICIAN_API UMagicianAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UMagicianAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
	
};
