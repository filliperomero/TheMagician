// Copyright Fillipe Romero


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "MagicianGameplayTags.h"
#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/MagicianPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChangedDelegate.Broadcast(GetMagicianPS()->GetSpellPoints());
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

	GetMagicianPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			SpellPointsChangedDelegate.Broadcast(Points);
		}
	);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const FMagicianGameplayTags GameplayTags = FMagicianGameplayTags::Get();
	const int32 CurrentSpellPoints = GetMagicianPS()->GetSpellPoints();
	FGameplayTag AbilityStatus;
	
	const FGameplayAbilitySpec* AbilitySpec = GetMagicianASC()->GetSpecFromAbilityTag(AbilityTag);

	if (!AbilityTag.IsValid() || AbilityTag.MatchesTag(GameplayTags.Abilities_None) || AbilitySpec == nullptr)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetMagicianASC()->GetStatusFromSpec(*AbilitySpec);
	}

	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;

	ShouldEnableButtons(AbilityStatus, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);

	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip);
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendPointsButton, bool& bShouldEnableEquipButton)
{
	const FMagicianGameplayTags GameplayTags = FMagicianGameplayTags::Get();

	bShouldEnableSpendPointsButton = false;
	bShouldEnableEquipButton = false;

	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		
		if (SpellPoints > 0) bShouldEnableSpendPointsButton = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		if (SpellPoints > 0) bShouldEnableSpendPointsButton = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		
		if (SpellPoints > 0) bShouldEnableSpendPointsButton = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
	{
		// Do nothing since we're already setting the bShouldEnableEquipButton to false in the beginning
	}
}
