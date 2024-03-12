// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MagicianAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, int32 /*AbilityLevel*/)
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, const FGameplayTag& /*SlotTag*/, const FGameplayTag& /*PrevSlotTag*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbilitySignature, const FGameplayTag& /*AbilityTag*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffectSignature, const FGameplayTag& /*AbilityTag*/, const bool /*bActivate*/)

struct FGameplayTag;

/**
 * 
 */
UCLASS()
class THEMAGICIAN_API UMagicianAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void ForEachAbility(const FForEachAbility& Delegate);
	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	void UpdateAbilityStatuses(int32 Level);
	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);
	void ClearSlot(FGameplayAbilitySpec* Spec);
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);
	static bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	// Can be called by the Client but will only run in the Server
	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	// Can be called by the Client but will only run in the Server
	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag);
	
	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusDelegate;
	FAbilityEquipped AbilityEquippedDelegate;
	FDeactivatePassiveAbilitySignature DeactivatePassiveAbilityDelegate;
	FActivatePassiveEffectSignature ActivatePassiveEffectDelegate;

	bool bStartupAbilitiesGiven { false };

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);
	
	/** Utility Functions */
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	bool SlotIsEmpty(const FGameplayTag& Slot);
	static bool AbilityHasSlot(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec);
	bool IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const;
	FGameplayAbilitySpec* GetAbilitySpecWithSlot(const FGameplayTag& Slot);
	void AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot);

protected:
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, const FGameplayTag& SlotTag, const FGameplayTag& PrevSlotTag);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, const bool bActivate);
	
	virtual void OnRep_ActivateAbilities() override;
};
