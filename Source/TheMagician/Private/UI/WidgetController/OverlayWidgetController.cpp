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
