// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/MagicianWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

UCLASS(BlueprintType, Blueprintable)
class THEMAGICIAN_API USpellMenuWidgetController : public UMagicianWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnPlayerStatChangedSignature SpellPointsChangedDelegate;
	
};
