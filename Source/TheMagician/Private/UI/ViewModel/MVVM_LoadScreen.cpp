// Copyright Fillipe Romero

#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/MagicianGameInstance.h"
#include "Game/MagicianGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->LoadSlotName = FString("LoadSlot_0");
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0, LoadSlot_0);
	
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->LoadSlotName = FString("LoadSlot_1");
	LoadSlot_1->SlotIndex = 1;
	LoadSlots.Add(1, LoadSlot_1);
	
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->LoadSlotName = FString("LoadSlot_2");
	LoadSlot_2->SlotIndex = 2;
	LoadSlots.Add(2, LoadSlot_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(const int Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 SlotIndex, const FString& EnteredName)
{
	AMagicianGameModeBase* MagicianGameMode = Cast<AMagicianGameModeBase>(UGameplayStatics::GetGameMode(this));

	LoadSlots[SlotIndex]->SetMapName(MagicianGameMode->DefaultMapName);
	LoadSlots[SlotIndex]->SetPlayerName(EnteredName);
	LoadSlots[SlotIndex]->SlotStatus = Taken;
	LoadSlots[SlotIndex]->PlayerStartTag = MagicianGameMode->DefaultPlayerStartTag;

	MagicianGameMode->SaveSlotData(LoadSlots[SlotIndex], SlotIndex);
	LoadSlots[SlotIndex]->InitializeSlot();

	UMagicianGameInstance* MagicianGameInstance = Cast<UMagicianGameInstance>(MagicianGameMode->GetGameInstance());
	MagicianGameInstance->LoadSlotName = LoadSlots[SlotIndex]->LoadSlotName;
	MagicianGameInstance->LoadSlotIndex = LoadSlots[SlotIndex]->SlotIndex;
	MagicianGameInstance->PlayerStartTag = MagicianGameMode->DefaultPlayerStartTag;
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 SlotIndex)
{
	LoadSlots[SlotIndex]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 SlotIndex)
{
	SlotSelected.Broadcast();
	
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		const bool bShouldDisable = LoadSlot.Key == SlotIndex;

		LoadSlot.Value->EnableSelectSlotButton.Broadcast(!bShouldDisable);
	}

	SelectedSlot = LoadSlots[SlotIndex];
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (!IsValid(SelectedSlot)) return;

	AMagicianGameModeBase::DeleteSlot(SelectedSlot->LoadSlotName, SelectedSlot->SlotIndex);

	SelectedSlot->SlotStatus = Vacant;
	SelectedSlot->InitializeSlot();
	SelectedSlot->EnableSelectSlotButton.Broadcast(true);

	SelectedSlot = nullptr;
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	if (!IsValid(SelectedSlot)) return;
	AMagicianGameModeBase* MagicianGameMode = Cast<AMagicianGameModeBase>(UGameplayStatics::GetGameMode(this));
	UMagicianGameInstance* MagicianGameInstance = MagicianGameMode->GetGameInstance<UMagicianGameInstance>();
	
	MagicianGameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;

	MagicianGameMode->TravelToMap(SelectedSlot);
}

void UMVVM_LoadScreen::LoadData()
{
	const AMagicianGameModeBase* MagicianGameMode = Cast<AMagicianGameModeBase>(UGameplayStatics::GetGameMode(this));
	
	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		const ULoadScreenSaveGame* SaveObject = MagicianGameMode->GetSaveSlotData(LoadSlot.Value->LoadSlotName, LoadSlot.Key);
		
		LoadSlot.Value->SlotStatus = SaveObject->SaveSlotStatus;
		LoadSlot.Value->SetPlayerName(SaveObject->PlayerName);
		LoadSlot.Value->SetMapName(SaveObject->MapName);
		LoadSlot.Value->PlayerStartTag = SaveObject->PlayerStartTag;
		
		LoadSlot.Value->InitializeSlot();
	}
}
