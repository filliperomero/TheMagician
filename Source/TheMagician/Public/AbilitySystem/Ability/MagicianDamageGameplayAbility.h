// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/MagicianGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "MagicianDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class THEMAGICIAN_API UMagicianDamageGameplayAbility : public UMagicianGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	FScalableFloat Damage;

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;
	
};
