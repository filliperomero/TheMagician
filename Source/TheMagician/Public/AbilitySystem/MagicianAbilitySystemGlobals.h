// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "MagicianAbilitySystemGlobals.generated.h"

UCLASS()
class THEMAGICIAN_API UMagicianAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	
};
