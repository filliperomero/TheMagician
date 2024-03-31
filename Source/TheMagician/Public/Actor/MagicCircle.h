// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

UCLASS()
class THEMAGICIAN_API AMagicCircle : public AActor
{
	GENERATED_BODY()

public:
	AMagicCircle();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDecalComponent> MagicCircleDecal;

protected:
	virtual void BeginPlay() override;
};
