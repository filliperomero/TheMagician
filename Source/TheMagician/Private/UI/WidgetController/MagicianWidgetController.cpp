// Copyright Fillipe Romero


#include "UI/WidgetController/MagicianWidgetController.h"

#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/MagicianAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/MagicianPlayerController.h"
#include "Player/MagicianPlayerState.h"

void UMagicianWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UMagicianWidgetController::BroadcastInitialValues()
{
}

void UMagicianWidgetController::BindCallbacksToDependencies()
{
}

void UMagicianWidgetController::BroadcastAbilityInfo()
{
	if (!GetMagicianASC()->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& AbilitySpec)
		{
			FMagicianAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(MagicianAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = MagicianAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
			Info.StatusTag = MagicianAbilitySystemComponent->GetStatusFromSpec(AbilitySpec);

			AbilityInfoDelegate.Broadcast(Info);
		}
	);

	GetMagicianASC()->ForEachAbility(BroadcastDelegate);
}

AMagicianPlayerController* UMagicianWidgetController::GetMagicianPC()
{
	if (MagicianPlayerController == nullptr)
	{
		MagicianPlayerController = Cast<AMagicianPlayerController>(PlayerController);
	}

	return MagicianPlayerController;
}

AMagicianPlayerState* UMagicianWidgetController::GetMagicianPS()
{
	if (MagicianPlayerState == nullptr)
	{
		MagicianPlayerState = Cast<AMagicianPlayerState>(PlayerState);
	}

	return MagicianPlayerState;
}

UMagicianAbilitySystemComponent* UMagicianWidgetController::GetMagicianASC()
{
	if (MagicianAbilitySystemComponent == nullptr)
	{
		MagicianAbilitySystemComponent = Cast<UMagicianAbilitySystemComponent>(AbilitySystemComponent);
	}

	return MagicianAbilitySystemComponent;
}

UMagicianAttributeSet* UMagicianWidgetController::GetMagicianAS()
{
	if (MagicianAttributeSet == nullptr)
	{
		MagicianAttributeSet = Cast<UMagicianAttributeSet>(AttributeSet);
	}

	return MagicianAttributeSet;
}
