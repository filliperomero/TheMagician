// Copyright Fillipe Romero


#include "MagicianAssetManager.h"
#include "MagicianGameplayTags.h"

UMagicianAssetManager& UMagicianAssetManager::Get()
{
	check(GEngine);
	
	UMagicianAssetManager* MagicianAssetManager = Cast<UMagicianAssetManager>(GEngine->AssetManager);

	return *MagicianAssetManager;
}

void UMagicianAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FMagicianGameplayTags::InitializeNativeGameplayTags();
}
