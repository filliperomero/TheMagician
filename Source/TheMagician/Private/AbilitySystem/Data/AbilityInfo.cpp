// Copyright Fillipe Romero


#include "AbilitySystem/Data/AbilityInfo.h"
#include "TheMagician/MagicianLogChannels.h"

FMagicianAbilityInfo UAbilityInfo::FindAbilityInfoByTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FMagicianAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag.MatchesTagExact(AbilityTag))
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogMagician, Error, TEXT("Can't find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}

	return FMagicianAbilityInfo();
}
