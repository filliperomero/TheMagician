// Copyright Fillipe Romero


#include "UI/WidgetController/MagicianWidgetController.h"

void UMagicianWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UMagicianWidgetController::BroadcastInitialValues()
{
}

void UMagicianWidgetController::BindCallbacksToDependencies()
{
}
