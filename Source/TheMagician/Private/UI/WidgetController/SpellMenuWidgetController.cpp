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
	[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
		{
			if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
			{
				SelectedAbility.StatusTag = StatusTag;
				bool bEnableSpendPoints = false;
				bool bEnableEquip = false;
				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
				
				FString Description;
				FString NextLevelDescription;
				GetMagicianASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
				SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
			}
		
			if (AbilityInfo)
			{
				FMagicianAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
				Info.StatusTag = StatusTag;

				AbilityInfoDelegate.Broadcast(Info);
			}
		}
	);

	GetMagicianASC()->AbilityEquippedDelegate.AddUObject(this, &ThisClass::OnAbilityEquipped);

	GetMagicianPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 Points)
		{
			SpellPointsChangedDelegate.Broadcast(Points);
			CurrentSpellPoints = Points;
			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(SelectedAbility.StatusTag, Points, bEnableSpendPoints, bEnableEquip);
			
			FString Description;
			FString NextLevelDescription;
			GetMagicianASC()->GetDescriptionsByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
		}
	);
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitingForEquipSelection)
	{
		const FMagicianAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag);
		StopWaitingForEquipSelectionDelegate.Broadcast(Info.AbilityTypeTag);
		bWaitingForEquipSelection = false;
	}
	
	const FMagicianGameplayTags GameplayTags = FMagicianGameplayTags::Get();
	const int32 SpellPoints = GetMagicianPS()->GetSpellPoints();
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

	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.StatusTag = AbilityStatus;

	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);
	
	FString Description;
	FString NextLevelDescription;
	GetMagicianASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetMagicianASC() == nullptr) return;

	// Call Server RPC so this just happens in the Server since spending points should only happens in the server
	GetMagicianASC()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
}

void USpellMenuWidgetController::GlobeDeselect()
{
	if (bWaitingForEquipSelection)
	{
		const FMagicianAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag);
		StopWaitingForEquipSelectionDelegate.Broadcast(Info.AbilityTypeTag);
		bWaitingForEquipSelection = false;
	}
	
	SelectedAbility.AbilityTag = FMagicianGameplayTags::Get().Abilities_None;
	SelectedAbility.StatusTag = FMagicianGameplayTags::Get().Abilities_Status_Locked;

	SpellGlobeSelectedDelegate.Broadcast(false, false,FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FMagicianAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag);

	WaitForEquipSelectionDelegate.Broadcast(Info.AbilityTypeTag);
	bWaitingForEquipSelection = true;

	const FGameplayTag SelectedStatus = GetMagicianASC()->GetStatusFromAbilityTag(SelectedAbility.AbilityTag);
	if (SelectedStatus.MatchesTagExact(FMagicianGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetMagicianASC()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityTypeTag)
{
	if (!bWaitingForEquipSelection || !SelectedAbility.AbilityTag.IsValid()) return;
	// Check Selected Ability against the slot's ability type. (don't equip an offensive spell in a passive slot and vice versa)
	const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(SelectedAbility.AbilityTag).AbilityTypeTag;
	if (!SelectedAbilityType.MatchesTagExact(AbilityTypeTag)) return;

	GetMagicianASC()->ServerEquipAbility(SelectedAbility.AbilityTag, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& SlotTag, const FGameplayTag& PrevSlotTag)
{
	bWaitingForEquipSelection = false;

	const FMagicianGameplayTags GameplayTags = FMagicianGameplayTags::Get();

	FMagicianAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PrevSlotTag;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	// Broadcast empty info if PrevSlotTag is a valid slot. Only if equipping on already-equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FMagicianAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	Info.StatusTag = StatusTag;
	Info.InputTag = SlotTag;
	AbilityInfoDelegate.Broadcast(Info);
	
	StopWaitingForEquipSelectionDelegate.Broadcast(Info.AbilityTypeTag);
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeselect();
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
