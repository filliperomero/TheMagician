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
	InitMaxHealth(100.f);
	InitMana(50.f);
	InitMaxMana(75.f);
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
