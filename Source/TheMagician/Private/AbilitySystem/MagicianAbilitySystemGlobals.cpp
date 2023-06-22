// Copyright Fillipe Romero


#include "AbilitySystem/MagicianAbilitySystemGlobals.h"

#include "MagicianAbilityTypes.h"

FGameplayEffectContext* UMagicianAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FMagicianGameplayEffectContext();
}
