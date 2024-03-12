// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/MagicianGameplayAbility.h"
#include "MagicianPassiveAbility.generated.h"

UCLASS()
class THEMAGICIAN_API UMagicianPassiveAbility : public UMagicianGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void ReceiveDeactivate(const FGameplayTag& AbilityTag);
protected:

private:
};
