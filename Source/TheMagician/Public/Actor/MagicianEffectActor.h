// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicianEffectActor.generated.h"

class UGameplayEffect;

UCLASS()
class THEMAGICIAN_API AMagicianEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMagicianEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	
};
