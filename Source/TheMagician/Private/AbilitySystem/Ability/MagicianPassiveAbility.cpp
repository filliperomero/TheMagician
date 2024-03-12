// Copyright Fillipe Romero

#include "AbilitySystem/Ability/MagicianPassiveAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MagicianAbilitySystemComponent.h"

void UMagicianPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UMagicianAbilitySystemComponent* MagicianASC = Cast<UMagicianAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		MagicianASC->DeactivatePassiveAbilityDelegate.AddUObject(this, &ThisClass::ReceiveDeactivate);
	}
}

void UMagicianPassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
