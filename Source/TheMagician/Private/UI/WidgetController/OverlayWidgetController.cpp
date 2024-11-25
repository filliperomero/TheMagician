// Copyright Fillipe Romero


#include "UI/WidgetController/OverlayWidgetController.h"

#include "MagicianGameplayTags.h"
#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/MagicianAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/MagicianPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
	
	OnHealthChanged.Broadcast(GetMagicianAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetMagicianAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetMagicianAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetMagicianAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	
	/** Bind to PlayerState Delegates */
	GetMagicianPS()->OnXPChangedDelegate.AddUObject(this, &ThisClass::OnXPChanged);
	GetMagicianPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel, bool bLevelUp)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel, bLevelUp);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMagicianAS()->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMagicianAS()->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMagicianAS()->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetMagicianAS()->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);
	
	GetMagicianASC()->EffectAssetTags.AddLambda(
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

	if (GetMagicianASC())
	{
		GetMagicianASC()->AbilityEquippedDelegate.AddUObject(this, &ThisClass::OnAbilityEquipped);
		/**
		 * We're not directly binding because there is a chance we bind to the delegate when the abilities has been given already, which would be too late.
		 * So if this case is true, we just call BroadcastAbilityInfo directly, otherwise we bind to the delegate 
		 */
		if (GetMagicianASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetMagicianASC()->AbilitiesGivenDelegate.AddUObject(this, &ThisClass::BroadcastAbilityInfo);
		}
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	// This will become a static cast and it'll not concern too much
	const ULevelUpInfo* LevelUpInfo = GetMagicianPS()->LevelUpInfo;

	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo. Please fill out MagicianPlayerState Blueprint"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	} 
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& SlotTag, const FGameplayTag& PrevSlotTag) const
{
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
}
