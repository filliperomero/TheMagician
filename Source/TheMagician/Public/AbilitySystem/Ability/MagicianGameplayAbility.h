// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MagicianGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class THEMAGICIAN_API UMagicianGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	FGameplayTag StartupInputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat Damage;
	
};
