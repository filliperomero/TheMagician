// Copyright Fillipe Romero


#include "UI/Widget/MagicianUserWidget.h"

void UMagicianUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
