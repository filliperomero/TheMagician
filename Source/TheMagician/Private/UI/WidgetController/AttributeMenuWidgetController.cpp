// Copyright Fillipe Romero


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/MagicianAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/MagicianPlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	check(AttributeInfo);
	
	for (auto& Pair : GetMagicianAS()->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value()); // Pair.Value() will return the FGameplayAttribute
	}
	
	AttributePointsChangedDelegate.Broadcast(GetMagicianPS()->GetAttributePoints());
	SpellPointsChangedDelegate.Broadcast(GetMagicianPS()->GetSpellPoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	check(AttributeInfo);
	
	for (auto& Pair : GetMagicianAS()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value()); // Pair.Value() will return the FGameplayAttribute
			}
		);
	}
	
	GetMagicianPS()->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			AttributePointsChangedDelegate.Broadcast(Points);
		}
	);

	GetMagicianPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			SpellPointsChangedDelegate.Broadcast(Points);
		}
	);
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetMagicianASC()->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FMagicianAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);

	AttributeInfoDelegate.Broadcast(Info);
}
