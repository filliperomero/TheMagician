// Copyright Fillipe Romero


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/MagicianAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/MagicianPlayerState.h"

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

	AMagicianPlayerState* MagicianPlayerState = CastChecked<AMagicianPlayerState>(PlayerState);
	const UMagicianAttributeSet* MagicianAttributeSet = CastChecked<UMagicianAttributeSet>(AttributeSet);

	/** Bind to OnXPChangedDelegate */
	MagicianPlayerState->OnXPChangedDelegate.AddUObject(this, &ThisClass::OnXPChanged);

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
	
	UMagicianAbilitySystemComponent* MagicianASC = CastChecked<UMagicianAbilitySystemComponent>(AbilitySystemComponent);

	MagicianASC->EffectAssetTags.AddLambda(
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
	
	/**
	 * We're not directly binding because there is a chance we bind to the delegate when the abilities has been given already, which would be too late.
	 * So if this case is true, we just call OnInitializeStartupAbilities directly, otherwise we bind to the delegate 
	 */
	if (MagicianASC->bStartupAbilitiesGiven)
	{
		OnInitializeStartupAbilities(MagicianASC);
	}
	else
	{
		MagicianASC->AbilitiesGivenDelegate.AddUObject(this, &ThisClass::OnInitializeStartupAbilities);
	}
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UMagicianAbilitySystemComponent* MagicianAbilitySystemComponent) const
{
	if (!MagicianAbilitySystemComponent->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this, MagicianAbilitySystemComponent](const FGameplayAbilitySpec& AbilitySpec)
		{
			FMagicianAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(MagicianAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = MagicianAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);

			AbilityInfoDelegate.Broadcast(Info);
		}
	);

	MagicianAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) const
{
	// This will become a static cast and it'll not concern too much
	const AMagicianPlayerState* MagicianPlayerState = CastChecked<AMagicianPlayerState>(PlayerState);
	const ULevelUpInfo* LevelUpInfo = MagicianPlayerState->LevelUpInfo;

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
