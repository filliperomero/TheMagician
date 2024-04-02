// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "MagicianDamageGameplayAbility.h"
#include "MagicianFireBlast.generated.h"

class AMagicianFireBall;

UCLASS()
class THEMAGICIAN_API UMagicianFireBlast : public UMagicianDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable)
	TArray<AMagicianFireBall*> SpawnFireBalls();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FireBlast")
	int32 NumFireBalls = 12;

private:
	UPROPERTY(EditDefaultsOnly, Category = "FireBlast")
	TSubclassOf<AMagicianFireBall> FireBallClass;
};
