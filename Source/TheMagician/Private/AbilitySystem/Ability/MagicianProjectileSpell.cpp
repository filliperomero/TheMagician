// Copyright Fillipe Romero


#include "AbilitySystem/Ability/MagicianProjectileSpell.h"
#include "Actor/MagicianProjectile.h"
#include "Interaction/CombatInterface.h"

void UMagicianProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthority(&ActivationInfo)) return;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		// TODO: Set the Projectile Rotation
		
		// We're not going to use the basic SpawnActor here because SpawnActorDeferred handles stuffs a little different.
		// Once the actor is finished spawning, all properties are already set
		AMagicianProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMagicianProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		// TODO: Give the Projectile a Gameplay Effect Spec for causing Damage

		Projectile->FinishSpawning(SpawnTransform);
	}
}