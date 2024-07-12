// Copyright Fillipe Romero

#include "UI/HUD/LoadScreenHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	checkf(LoadScreenViewModelClass, TEXT("LoadScreenViewModelClass uninitialized, please fill out BP_LoadScreenHUD"));
	checkf(LoadScreenWidgetClass, TEXT("LoadScreenWidgetClass uninitialized, please fill out BP_LoadScreenHUD"));

	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	LoadScreenViewModel->InitializeLoadSlots();
	
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();

	// That way, we make sure we are calling this in the right moment
	LoadScreenWidget->BlueprintInitializeWidget();

	LoadScreenViewModel->LoadData();
}
