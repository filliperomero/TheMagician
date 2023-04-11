// Copyright Fillipe Romero


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/MagicianAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	const UMagicianAttributeSet* MagicianAttributeSet = CastChecked<UMagicianAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(MagicianAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(MagicianAttributeSet->GetMaxHealth());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	const UMagicianAttributeSet* MagicianAttributeSet = CastChecked<UMagicianAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		MagicianAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		MagicianAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}
