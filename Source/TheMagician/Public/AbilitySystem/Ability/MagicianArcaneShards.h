// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "MagicianDamageGameplayAbility.h"
#include "MagicianArcaneShards.generated.h"

UCLASS()
class THEMAGICIAN_API UMagicianArcaneShards : public UMagicianDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxNumShards = 11;
};
