// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/MagicianGameplayAbility.h"
#include "MagicianDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class THEMAGICIAN_API UMagicianDamageGameplayAbility : public UMagicianGameplayAbility
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
	
};
