// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/MagicianWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class THEMAGICIAN_API UAttributeMenuWidgetController : public UMagicianWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	
};
