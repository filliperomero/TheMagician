// Copyright Fillipe Romero


#include "AbilitySystem/MagicianAbilitySystemComponent.h"

/** This Function will run once the ASC is initialized. */
void UMagicianAbilitySystemComponent::AbilityActorInfoSet()
{
	// Bind to Delegate
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::EffectApplied);
}

void UMagicianAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                    const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Blue, FString("Effect Applied"));
}
