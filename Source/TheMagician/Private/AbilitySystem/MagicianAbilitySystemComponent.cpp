// Copyright Fillipe Romero


#include "AbilitySystem/MagicianAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Ability/MagicianGameplayAbility.h"
#include "Interaction/PlayerInterface.h"
#include "TheMagician/MagicianLogChannels.h"

/** This Function will run once the ASC is initialized. */
void UMagicianAbilitySystemComponent::AbilityActorInfoSet()
{
	// Bind to Delegate
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::ClientEffectApplied);
}

void UMagicianAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		
		if (const UMagicianGameplayAbility* MagicianAbility = Cast<UMagicianGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(MagicianAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}

	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast(this);
}

void UMagicianAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> PassiveAbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(PassiveAbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UMagicianAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// We're just going to activate the ones that has the InputTag
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// Set a bool in the AbilitySpec that is keeping track of whether or not its particular input is pressed
			AbilitySpecInputPressed(AbilitySpec);
			// Just try to activate if is not already activated
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UMagicianAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// Set a bool in the AbilitySpec that is keeping track of whether or not its particular input is released
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UMagicianAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	// This will lock the list of abilities until we finish the loop (this is a good practice).
	// This is to make sure we're not removing or blocking any abilities while we're looping it
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogMagician, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

void UMagicianAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (!GetAvatarActor()->Implements<UPlayerInterface>() || IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) <= 0) return;

	// Inform the server that we want to upgrade the attribute since it's the server's job to do it
	ServerUpgradeAttribute(AttributeTag);
}

void UMagicianAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	// Checks if the Player really have points to spend
	if (!GetAvatarActor()->Implements<UPlayerInterface>() || IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) <= 0) return;
	
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	// Remove 1 point from the Player's Attribute Points
	IPlayerInterface::Execute_AddAttributePoints(GetAvatarActor(), -1);
}

FGameplayTag UMagicianAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}

	return FGameplayTag();
}

FGameplayTag UMagicianAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

void UMagicianAbilitySystemComponent::OnRep_ActivateAbilities()
{
	/**
	 * Since AddCharacterAbilities only runs on the server, we make use of this function so when the abilities are given, it will replicate to
	 * all clients and we can act on that here. So we can call the AbilitiesGivenDelegate here as well.
	 */
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true; // Since we don't replicate this variable, even if we set to true before, it will be false for the first time on the client
		AbilitiesGivenDelegate.Broadcast(this);
	}
}

void UMagicianAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                         const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
