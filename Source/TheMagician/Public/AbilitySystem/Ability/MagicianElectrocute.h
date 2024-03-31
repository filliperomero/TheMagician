// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "MagicianBeamSpell.h"
#include "MagicianElectrocute.generated.h"

/**
 * 
 */
UCLASS()
class THEMAGICIAN_API UMagicianElectrocute : public UMagicianBeamSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
