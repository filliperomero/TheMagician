// Copyright Fillipe Romero


#include "AbilitySystem/Ability/MagicianGameplayAbility.h"

#include "AbilitySystem/MagicianAttributeSet.h"

FString UMagicianGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name - Some Description Here", Level);
}

FString UMagicianGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Causes much more damage.</>"), Level);
}

FString UMagicianGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level: </><Level>%d</>"), Level);
}

float UMagicianGameplayAbility::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;
	const UGameplayEffect* CostEffect = GetCostGameplayEffect();
	
	if (CostEffect == nullptr) return ManaCost;
		
	for (FGameplayModifierInfo Modifier : CostEffect->Modifiers)
	{
		if (Modifier.Attribute == UMagicianAttributeSet::GetManaAttribute())
		{
			// This will only works if the magnitude is a ScalableFloat
			Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
			break; // Already found the Mana Cost
		}
	}

	return FMath::Abs(ManaCost);
}

float UMagicianGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();

	if (CooldownEffect == nullptr) return Cooldown;

	CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	
	return Cooldown;
}
