// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "MagicianAbilityTypes.h"
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

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(
		AActor* TargetActor = nullptr,
		FVector InRadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockbackDirection = false,
		FVector KnockbackDirectionOverride = FVector::ZeroVector,
		bool bOverrideDeathImpulse = false,
		FVector DeathImpulseDirectionOverride = FVector::ZeroVector,
		bool bOverridePitch = false,
		float PitchOverride = 0.f
	) const;

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	bool bIsRadialDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Damage)
	float RadialDamageOuterRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = Debuff)
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = Debuff)
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = Debuff)
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = Debuff)
	float DebuffDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	float DeathImpulseMagnitude = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	float KnockbackForceMagnitude = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	float KnockbackChance = 0.f;
	
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;
	
};
