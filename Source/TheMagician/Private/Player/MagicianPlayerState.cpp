// Copyright Fillipe Romero


#include "Player/MagicianPlayerState.h"

#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/MagicianAttributeSet.h"
#include "Net/UnrealNetwork.h"

AMagicianPlayerState::AMagicianPlayerState()
{
	// How often the Server will try to update clients
	NetUpdateFrequency = 100.f;

	AbilitySystemComponent = CreateDefaultSubobject<UMagicianAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UMagicianAttributeSet>("AttributeSet");
}

void AMagicianPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMagicianPlayerState, Level);
	DOREPLIFETIME(AMagicianPlayerState, XP);
}

UAbilitySystemComponent* AMagicianPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMagicianPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AMagicianPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AMagicianPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AMagicianPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AMagicianPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level);
}

void AMagicianPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}
