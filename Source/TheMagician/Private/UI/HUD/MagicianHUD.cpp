// Copyright Fillipe Romero


#include "UI/HUD/MagicianHUD.h"
#include "UI/Widget/MagicianUserWidget.h"

void AMagicianHUD::BeginPlay()
{
	Super::BeginPlay();

	check(OverlayWidgetClass);
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	Widget->AddToViewport();
}
