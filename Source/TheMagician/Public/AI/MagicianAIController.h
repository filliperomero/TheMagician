// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MagicianAIController.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class THEMAGICIAN_API AMagicianAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMagicianAIController();

protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
