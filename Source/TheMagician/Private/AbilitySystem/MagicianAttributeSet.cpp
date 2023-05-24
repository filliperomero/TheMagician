// Copyright Fillipe Romero

#include "AbilitySystem/MagicianAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UMagicianAttributeSet::UMagicianAttributeSet()
{
	InitHealth(50.f);
	InitMana(50.f);
}
	
void UMagicianAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, Vitality, COND_None, REPNOTIFY_Always);

	// Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	// Register Health to replicate
	// COND_None = We always want to replicate
	// REPNOTIFY_Always = If the value is set in the server, it will always replicate even if the variable is the same
	// Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMagicianAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

void UMagicianAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	
	if (Attribute == GetManaAttribute())
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
}

void UMagicianAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source = Causer of the effect; Target = target of the effect (owner of this AS)

	// Set Source values
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	Props.SourceAvatarActor = Props.SourceASC->GetAvatarActor();
	Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
	
	if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
	{
		if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			Props.SourceController = Pawn->GetController();
	}
	
	if (Props.SourceController)
		Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());

	// Set Target values
	Props.TargetAvatarActor = Data.Target.GetAvatarActor();
	Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
	Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
	Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
}

void UMagicianAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
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

void UMagicianAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, Strength, OldStrength);
}

void UMagicianAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, Intelligence, OldIntelligence);
}

void UMagicianAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, Resilience, OldResilience);
}

void UMagicianAttributeSet::OnRep_Vitality(const FGameplayAttributeData& OldVitality) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, Vitality, OldVitality);
}

void UMagicianAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, Armor, OldArmor);
}

void UMagicianAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UMagicianAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, BlockChance, OldBlockChance);
}

void UMagicianAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UMagicianAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UMagicianAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UMagicianAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UMagicianAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMagicianAttributeSet, ManaRegeneration, OldManaRegeneration);
}
