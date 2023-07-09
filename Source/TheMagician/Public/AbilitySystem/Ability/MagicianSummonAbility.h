// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/MagicianGameplayAbility.h"
#include "MagicianSummonAbility.generated.h"

UCLASS()
class THEMAGICIAN_API UMagicianSummonAbility : public UMagicianGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	UFUNCTION(BlueprintPure)
	TSubclassOf<APawn> GetRandomMinionClass();

	UPROPERTY(EditDefaultsOnly, Category = "Summon Properties")
	int32 NumMinions { 5 };

	UPROPERTY(EditDefaultsOnly, Category = "Summon Properties")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Summon Properties")
	float MinSpawnDistance { 150.f };

	UPROPERTY(EditDefaultsOnly, Category = "Summon Properties")
	float MaxSpawnDistance { 400.f };

	UPROPERTY(EditDefaultsOnly, Category = "Summon Properties")
	float SpawnSpread { 90.f };
};
