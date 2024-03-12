// Copyright Fillipe Romero

#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UMagicianAbilitySystemComponent* MagicianASC = Cast<UMagicianAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		MagicianASC->ActivatePassiveEffectDelegate.AddUObject(this, &ThisClass::OnPassiveActivate);
	}
	// Since ASC can be nullptr because it can be too soon, we make a delegate to broadcast the ASC when the ASC is ready
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UMagicianAbilitySystemComponent* MagicianASC = Cast<UMagicianAbilitySystemComponent>(ASC))
			{
				MagicianASC->ActivatePassiveEffectDelegate.AddUObject(this, &ThisClass::OnPassiveActivate);
			}
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, const bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate)
		{
			if (!IsActive()) Activate();
		}
		else
		{
			Deactivate();
		}
	}
}
