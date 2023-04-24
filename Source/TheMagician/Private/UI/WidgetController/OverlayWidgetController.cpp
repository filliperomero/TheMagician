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

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		MagicianAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		MagicianAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		MagicianAttributeSet->GetManaAttribute()).AddUObject(this, &ThisClass::ManaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		MagicianAttributeSet->GetMaxManaAttribute()).AddUObject(this, &ThisClass::MaxManaChanged);

	UMagicianAbilitySystemComponent* MagicianAbilitySystemComponent = CastChecked<UMagicianAbilitySystemComponent>(AbilitySystemComponent);

	MagicianAbilitySystemComponent->EffectAssetTags.AddLambda(
		[](const FGameplayTagContainer& AssetTags)
		{
			// We're using reference here so we don't make a copy of it
			for (const FGameplayTag& Tag : AssetTags)
			{
				const FString Message = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Message);
			}
		}
	);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
