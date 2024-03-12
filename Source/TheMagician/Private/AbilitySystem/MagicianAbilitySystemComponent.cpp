// Copyright Fillipe Romero


#include "AbilitySystem/MagicianAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MagicianGameplayTags.h"
#include "AbilitySystem/MagicianAbilitySystemLibrary.h"
#include "AbilitySystem/Ability/MagicianGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
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
			AbilitySpec.DynamicAbilityTags.AddTag(FMagicianGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}

	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
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

	FScopedAbilityListLock ActiveScopeLock(*this);
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

	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			// Set a bool in the AbilitySpec that is keeping track of whether or not its particular input is released
			AbilitySpecInputReleased(AbilitySpec);
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UMagicianAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// We're just going to activate the ones that has the InputTag
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// Set a bool in the AbilitySpec that is keeping track of whether or not its particular input is pressed
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
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

void UMagicianAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UMagicianAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());

	if (AbilityInfo == nullptr) return;

	for (const FMagicianAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (Level < Info.LevelRequirement || !Info.AbilityTag.IsValid()) continue;

		// If it's nullptr, it means that it is not in our activatable abilities (which means that it was not given yet).
		// So we need to give the ability to the Character
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(FMagicianGameplayTags::Get().Abilities_Status_Eligible);

			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec); // Force AbilitySpec to replicate
			// Call Client's RPC to update in the Client the information
			ClientUpdateAbilityStatus(Info.AbilityTag, FMagicianGameplayTags::Get().Abilities_Status_Eligible, 1);
		}
	}
}

bool UMagicianAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UMagicianGameplayAbility* MagicianAbility = Cast<UMagicianGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = MagicianAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = MagicianAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			
			return true;
		}
	}

	const UAbilityInfo* AbilityInfo = UMagicianAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FMagicianGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	} else
	{
		OutDescription = UMagicianGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoByTag(AbilityTag).LevelRequirement);
	}
	OutNextLevelDescription = FString();
	return false;
}

void UMagicianAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*Spec);
	if (!Slot.IsValid()) return;
	
	Spec->DynamicAbilityTags.RemoveTag(Slot);
	MarkAbilitySpecDirty(*Spec);
}

void UMagicianAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	if (!Slot.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&Spec, Slot))
		{
			ClearSlot(&Spec);
		}
	}
}

bool UMagicianAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	if (Spec == nullptr || !Slot.IsValid()) return false;
	
	for (FGameplayTag Tag : Spec->DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}

	return false;
}

void UMagicianAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FMagicianGameplayTags& GameplayTags = FMagicianGameplayTags::Get();
		const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& StatusTag = GetStatusFromSpec(*AbilitySpec);
		
		if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			// There is an Ability in this slot already. Deactivate and clear its slot.
			if (!SlotIsEmpty(SlotTag))
			{
				if (FGameplayAbilitySpec* SpecWithSlot = GetAbilitySpecWithSlot(SlotTag))
				{
					// This indicates we're trying to activate the same ability to the slot
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, SlotTag, PrevSlot);
						return;
					}

					if (IsPassiveAbility(*SpecWithSlot))
					{
						MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot), false);
						DeactivatePassiveAbilityDelegate.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
					}

					// Clear this ability Slot, just in case is a different slot
					ClearSlot(SpecWithSlot);
				}
			}

			// Ability doesn't yet have a slot (It's not active)
			if (!AbilityHasAnySlot(*AbilitySpec))
			{
				if (IsPassiveAbility(*AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag, true);
				}
				AbilitySpec->DynamicAbilityTags.RemoveTag(GetStatusFromSpec(*AbilitySpec));
				AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
			}

			AssignSlotToAbility(*AbilitySpec, SlotTag);
			MarkAbilitySpecDirty(*AbilitySpec);
		}

		ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, SlotTag, PrevSlot);
	}
}

void UMagicianAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag);
	if (AbilitySpec == nullptr) return;

	if(GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddSpellPoints(GetAvatarActor(), -1);
	}

	const FMagicianGameplayTags GameplayTags = FMagicianGameplayTags::Get();
	FGameplayTag StatusTag = GetStatusFromSpec(*AbilitySpec);

	if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
		AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
		StatusTag = GameplayTags.Abilities_Status_Unlocked;
	}
	else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		AbilitySpec->Level += 1;
	}

	ClientUpdateAbilityStatus(AbilityTag, StatusTag, AbilitySpec->Level);
	MarkAbilitySpecDirty(*AbilitySpec);
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

FGameplayAbilitySpec* UMagicianAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag)) return &AbilitySpec;
		}
	}

	return nullptr;
}

FGameplayTag UMagicianAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*Spec);
	}

	return FGameplayTag();
}

FGameplayTag UMagicianAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}

	return FGameplayTag();
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

FGameplayTag UMagicianAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return StatusTag;
		}
	}

	return FGameplayTag();
}

bool UMagicianAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, Slot)) return false;
	}

	return true;
}

bool UMagicianAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot)
{
	return AbilitySpec.DynamicAbilityTags.HasTagExact(Slot);
}

bool UMagicianAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec)
{
	return AbilitySpec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

bool UMagicianAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
	const UAbilityInfo* AbilityInfo = UMagicianAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(AbilitySpec);
	const FMagicianAbilityInfo& Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	const FGameplayTag AbilityType = Info.AbilityTypeTag;
	
	return AbilityType.MatchesTagExact(FMagicianGameplayTags::Get().Abilities_Type_Passive);
}

FGameplayAbilitySpec* UMagicianAbilitySystemComponent::GetAbilitySpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);
	
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(Slot))
		{
			return &AbilitySpec;
		}
	}

	return nullptr;
}

void UMagicianAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot)
{
	ClearSlot(&AbilitySpec);

	AbilitySpec.DynamicAbilityTags.AddTag(Slot);
}

void UMagicianAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UMagicianAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& SlotTag, const FGameplayTag& PrevSlotTag)
{
	AbilityEquippedDelegate.Broadcast(AbilityTag, StatusTag, SlotTag, PrevSlotTag);
}

void UMagicianAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag, const bool bActivate)
{
	ActivatePassiveEffectDelegate.Broadcast(AbilityTag, bActivate);
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
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UMagicianAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                         const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}
