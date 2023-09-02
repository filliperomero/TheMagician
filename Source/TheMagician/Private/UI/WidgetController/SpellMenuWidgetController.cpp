// Copyright Fillipe Romero


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetMagicianASC()->AbilityStatusDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
			{
				if (AbilityInfo)
				{
					FMagicianAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
					Info.StatusTag = StatusTag;

					AbilityInfoDelegate.Broadcast(Info);
				}
			}
		);
}
