// Copyright Fillipe Romero

#include "Actor/MagicianFireBall.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayCueManager.h"
#include "MagicianGameplayTags.h"
#include "AbilitySystem/MagicianAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"

void AMagicianFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AMagicianFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UMagicianAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}

void AMagicianFireBall::OnHit()
{
	if (IsValid(GetOwner()))
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FMagicianGameplayTags::Get().GameplayCue_FireBlast, CueParams);
	}
	
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	
	bHit = true;
}
