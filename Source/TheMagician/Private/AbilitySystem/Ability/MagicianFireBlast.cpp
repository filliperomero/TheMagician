// Copyright Fillipe Romero

#include "AbilitySystem/Ability/MagicianFireBlast.h"
#include "AbilitySystem/MagicianAbilitySystemLibrary.h"
#include "Actor/MagicianFireBall.h"

FString UMagicianFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		"<Title>FIRE BLAST</>\n\n"
		
		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		
		"<Default>Launches %d fireballs in all directions, each coming back and exploding upon return, causing </>"
		"<Damage>%d</>"
		"<Default> radial fire damage with a chance to burn</>"),
		Level,
		ManaCost,
		Cooldown,
		NumFireBalls,
		ScaledDamage
	);
}

FString UMagicianFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		
		"<Default>Launches %d fireballs in all directions, each coming back and exploding upon return, causing </>"
		"<Damage>%d</>"
		"<Default> radial fire damage with a chance to burn</>"),
		Level,
		ManaCost,
		Cooldown,
		NumFireBalls,
		ScaledDamage
	);
}

TArray<AMagicianFireBall*> UMagicianFireBlast::SpawnFireBalls()
{
	TArray<AMagicianFireBall*> FireBalls;
	
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector ActorLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	
	TArray<FRotator> Rotators = UMagicianAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireBalls);

	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(ActorLocation);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		AMagicianFireBall* FireBall = GetWorld()->SpawnActorDeferred<AMagicianFireBall>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		
		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
		FireBall->SetOwner(GetAvatarActorFromActorInfo());

		FDamageEffectParams ExplosionDamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->ExplosionDamageParams = ExplosionDamageEffectParams;

		FireBalls.Add(FireBall);
		
		FireBall->FinishSpawning(SpawnTransform);
	}

	return FireBalls;
}
