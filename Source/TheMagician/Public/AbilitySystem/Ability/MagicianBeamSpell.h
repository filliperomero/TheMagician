// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "MagicianDamageGameplayAbility.h"
#include "MagicianBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class THEMAGICIAN_API UMagicianBeamSpell : public UMagicianDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);

	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);

protected:
	UPROPERTY(BlueprintReadWrite, Category = Beam)
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category = Beam)
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category = Beam)
	TObjectPtr<APlayerController> OwnerPlayerController;

	UPROPERTY(BlueprintReadWrite, Category = Beam)
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = Beam)
	int32 MaxNumShockTargets = 5;
};
