// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/MagicianProjectileSpell.h"
#include "MagicianFireBolt.generated.h"

UCLASS()
class THEMAGICIAN_API UMagicianFireBolt : public UMagicianProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
