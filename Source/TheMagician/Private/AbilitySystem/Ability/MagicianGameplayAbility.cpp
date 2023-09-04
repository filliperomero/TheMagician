// Copyright Fillipe Romero


#include "AbilitySystem/Ability/MagicianGameplayAbility.h"

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
