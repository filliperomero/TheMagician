// Copyright Fillipe Romero


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/MagicianAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/MagicianPlayerState.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	check(AttributeInfo);

	const UMagicianAttributeSet* AS = CastChecked<UMagicianAttributeSet>(AttributeSet);

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value()); // Pair.Value() will return the FGameplayAttribute
	}

	const AMagicianPlayerState* MagicianPlayerState = CastChecked<AMagicianPlayerState>(PlayerState);
	
	AttributePointsChangedDelegate.Broadcast(MagicianPlayerState->GetAttributePoints());
	SpellPointsChangedDelegate.Broadcast(MagicianPlayerState->GetSpellPoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	check(AttributeInfo);

	const UMagicianAttributeSet* AS = CastChecked<UMagicianAttributeSet>(AttributeSet);

	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value()); // Pair.Value() will return the FGameplayAttribute
			}
		);
	}

	AMagicianPlayerState* MagicianPlayerState = CastChecked<AMagicianPlayerState>(PlayerState);
	MagicianPlayerState->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			AttributePointsChangedDelegate.Broadcast(Points);
		}
	);

	MagicianPlayerState->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			SpellPointsChangedDelegate.Broadcast(Points);
		}
	);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FMagicianAttributeInfo Info = AttributeInfo->FindAttributeInfoByTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);

	AttributeInfoDelegate.Broadcast(Info);
}
