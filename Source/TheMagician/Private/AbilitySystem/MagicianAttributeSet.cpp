// Copyright Fillipe Romero


#include "AbilitySystem/MagicianAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UMagicianAttributeSet::UMagicianAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(50.f);
	InitMaxMana(50.f);
}

void UMagicianAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Register Health to replicate
	// COND_None = We always want to replicate
	// REPNOTIFY_Always = If the value is set in the server, it will always replicate even if the variable is the same
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UMagicianAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// Informs the Ability System that we're replicating a value
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, Health, OldHealth);
}

void UMagicianAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, MaxHealth, OldMaxHealth);
}

void UMagicianAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, Mana, OldMana);
}

void UMagicianAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, MaxMana, OldMaxMana);
}
