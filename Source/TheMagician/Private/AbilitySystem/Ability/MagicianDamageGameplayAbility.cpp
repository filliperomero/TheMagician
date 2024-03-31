// Copyright Fillipe Romero


#include "AbilitySystem/Ability/MagicianDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UMagicianDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	
	// Add Damages by Damage Type
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaledDamage);

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FDamageEffectParams UMagicianDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
	FVector InRadialDamageOrigin, bool bOverrideKnockbackDirection, FVector KnockbackDirectionOverride,
	bool bOverrideDeathImpulse, FVector DeathImpulseDirectionOverride, bool bOverridePitch, float PitchOverride) const
{
	FDamageEffectParams DamageEffectParams;
	DamageEffectParams.WorldContextObj = GetAvatarActorFromActorInfo();
	DamageEffectParams.DamageGameplayEffectClass = DamageEffectClass;
	DamageEffectParams.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	DamageEffectParams.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	DamageEffectParams.AbilityLevel = GetAbilityLevel();
	DamageEffectParams.DamageType = DamageType;
	DamageEffectParams.DebuffChance = DebuffChance;
	DamageEffectParams.DebuffDamage = DebuffDamage;
	DamageEffectParams.DebuffFrequency = DebuffFrequency;
	DamageEffectParams.DebuffDuration = DebuffDuration;
	DamageEffectParams.DeathImpulseMagnitude = DeathImpulseMagnitude;
	DamageEffectParams.KnockbackForceMagnitude = KnockbackForceMagnitude;
	DamageEffectParams.KnockbackForceChance = KnockbackChance;

	if (bIsRadialDamage)
	{
		DamageEffectParams.bIsRadialDamage = bIsRadialDamage;
		DamageEffectParams.RadialDamageOrigin = InRadialDamageOrigin;
		DamageEffectParams.RadialDamageInnerRadius = RadialDamageInnerRadius;
		DamageEffectParams.RadialDamageOuterRadius = RadialDamageOuterRadius;
	}

	FVector ToTargetVector = FVector::ZeroVector;
	
	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotation.Pitch = bOverridePitch ? PitchOverride : 45.f;
		ToTargetVector = Rotation.Vector();
	}

	if (bOverrideKnockbackDirection)
	{
		KnockbackDirectionOverride.Normalize();
		DamageEffectParams.KnockbackForce = KnockbackDirectionOverride * KnockbackForceMagnitude;
		
		if (bOverridePitch)
		{
			FRotator KnockbackRotation = KnockbackDirectionOverride.Rotation();
			KnockbackRotation.Pitch = PitchOverride;
			DamageEffectParams.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
		}
	}
	else
	{
		DamageEffectParams.KnockbackForce = ToTargetVector * KnockbackForceMagnitude;
	}

	if (bOverrideDeathImpulse)
	{
		DeathImpulseDirectionOverride.Normalize();
		DamageEffectParams.DeathImpulse = DeathImpulseDirectionOverride * DeathImpulseMagnitude;
		if (bOverridePitch)
		{
			FRotator DeathImpulseRotation = DeathImpulseDirectionOverride.Rotation();
			DeathImpulseRotation.Pitch = PitchOverride;
			DamageEffectParams.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
		}
	}
	else
	{
		DamageEffectParams.DeathImpulse = ToTargetVector * DeathImpulseMagnitude;
	}

	return DamageEffectParams;
}

float UMagicianDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FTaggedMontage UMagicianDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.Num() == 0) return FTaggedMontage();

	const int32 Selection = FMath::RandRange(0, TaggedMontages.Num() - 1);

	return TaggedMontages[Selection];
}
