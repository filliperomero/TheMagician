// Copyright Fillipe Romero


#include "Game/MagicianGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Game/MagicianGameInstance.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AMagicianGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	DeleteSlot(LoadSlot->LoadSlotName, SlotIndex);
	
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SaveSlotStatus = Taken;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex);
}

bool AMagicianGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
		
		return true;
	}

	return false;
}

ULoadScreenSaveGame* AMagicianGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;
	
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}

	return Cast<ULoadScreenSaveGame>(SaveGameObject);
}

void AMagicianGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	if (!IsValid(Slot)) return;
	
	// const FString SlotName = Slot->LoadSlotName;
	// const int32 SlotIndex = Slot->SlotIndex;
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

AActor* AMagicianGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UMagicianGameInstance* MagicianGameInstance = Cast<UMagicianGameInstance>(GetGameInstance());
	
	TArray<AActor*> Actors; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);

	if (Actors.IsEmpty()) return nullptr;

	AActor* SelectedActor = Actors[0];
	for (AActor* Actor : Actors)
	{
		if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
		{
			if (PlayerStart->PlayerStartTag == MagicianGameInstance->PlayerStartTag)
			{
				SelectedActor = PlayerStart;
				break;
			}
		}
	}

	return SelectedActor;
}

void AMagicianGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}
