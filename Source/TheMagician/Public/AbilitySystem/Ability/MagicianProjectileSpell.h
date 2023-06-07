// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/MagicianGameplayAbility.h"
#include "MagicianProjectileSpell.generated.h"

class AMagicianProjectile;

UCLASS()
class THEMAGICIAN_API UMagicianProjectileSpell : public UMagicianGameplayAbility
{
	GENERATED_BODY()

public:

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = Projectile)
	void SpawnProjectile(const FVector& ProjectileTargetLocation);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMagicianProjectile> ProjectileClass;
	
};
