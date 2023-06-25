// Copyright Fillipe Romero


#include "Actor/MagicianEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AMagicianEffectActor::AMagicianEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AMagicianEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMagicianEffectActor::ApplyEffectsToTarget(AActor* TargetActor, TArray<FGameplayEffectBase> GameplayEffects)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	
	// Avoid the need to cast to IAbilitySystemInterface and checking it, then getting the AbilitySystem from it
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (!IsValid(TargetASC)) return;

	for (const auto GameplayEffect : GameplayEffects)
	{
		ApplyEffect(TargetASC, GameplayEffect);
	}
}

void AMagicianEffectActor::ApplyEffect(UAbilitySystemComponent* TargetActorASC, FGameplayEffectBase GameplayEffect)
{
	if (!IsValid(TargetActorASC)) return;
	
	check(GameplayEffect.GameplayEffectClass);

	FGameplayEffectContextHandle EffectContextHandle = TargetActorASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this); // If we want to know what object caused this effect, we can use the sourceObject

	const FGameplayEffectSpecHandle EffectSpecHandle = TargetActorASC->MakeOutgoingSpec(GameplayEffect.GameplayEffectClass, ActorLevel, EffectContextHandle);
	// Apply the Gameplay Effect
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetActorASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

	if (bIsInfinite && GameplayEffect.EffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetActorASC);

	if (bDestroyOnEffectApplication) Destroy();
}

void AMagicianEffectActor::RemoveEffectFromTarget(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	if (!IsValid(TargetASC)) return;

	TArray<FActiveGameplayEffectHandle> HandlesToRemove;
	for (auto HandlePair : ActiveEffectHandles)
	{
		if (TargetASC == HandlePair.Value)
		{
			// If we don't pass any StacksToRemove, it'll remove everything
			TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
			HandlesToRemove.Add(HandlePair.Key);
		}
	}

	for (auto& Handle : HandlesToRemove)
	{
		ActiveEffectHandles.FindAndRemoveChecked(Handle);
	}
}

void AMagicianEffectActor::OnOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	for (const auto GameplayEffect : GameplayEffectList)
	{
		if (GameplayEffect.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
			ApplyEffect(TargetASC, GameplayEffect);
	}
}

void AMagicianEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	for (const auto GameplayEffect : GameplayEffectList)
	{
		if (GameplayEffect.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
			ApplyEffect(TargetASC, GameplayEffect);

		if (GameplayEffect.EffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
			RemoveEffectFromTarget(TargetActor);
	}
}
