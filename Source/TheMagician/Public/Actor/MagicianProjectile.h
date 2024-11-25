// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "MagicianAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "MagicianProjectile.generated.h"

class UNiagaraSystem;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class THEMAGICIAN_API AMagicianProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AMagicianProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FDamageEffectParams DamageEffectParams;

	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	UFUNCTION(BlueprintCallable)
	virtual void OnHit();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool IsValidOverlap(AActor* OtherActor);

	bool bHit = false;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
private:
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VFX, meta=(AllowPrivateAccess = true))
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SFX, meta=(AllowPrivateAccess = true))
	TObjectPtr<USoundBase> ImpactSound;
	
	UPROPERTY(EditAnywhere, Category = SFX)
	TObjectPtr<USoundBase> LoopingSound;
	
};
