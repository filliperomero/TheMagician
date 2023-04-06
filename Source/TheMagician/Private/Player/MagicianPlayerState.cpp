// Copyright Fillipe Romero


#include "Player/MagicianPlayerState.h"

#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/MagicianAttributeSet.h"

AMagicianPlayerState::AMagicianPlayerState()
{
	// How often the Server will try to update clients
	NetUpdateFrequency = 100.f;

	AbilitySystemComponent = CreateDefaultSubobject<UMagicianAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UMagicianAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AMagicianPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
