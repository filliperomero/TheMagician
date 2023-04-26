// Copyright Fillipe Romero


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/MagicianAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	const UMagicianAttributeSet* MagicianAttributeSet = CastChecked<UMagicianAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(MagicianAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(MagicianAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(MagicianAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(MagicianAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	const UMagicianAttributeSet* MagicianAttributeSet = CastChecked<UMagicianAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MagicianAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MagicianAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MagicianAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MagicianAttributeSet->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);

	UMagicianAbilitySystemComponent* MagicianAbilitySystemComponent = CastChecked<UMagicianAbilitySystemComponent>(AbilitySystemComponent);

	MagicianAbilitySystemComponent->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			// We're using reference here so we don't make a copy of it
			for (const FGameplayTag& Tag : AssetTags)
			{
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				
				if (Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		}
	);
}
