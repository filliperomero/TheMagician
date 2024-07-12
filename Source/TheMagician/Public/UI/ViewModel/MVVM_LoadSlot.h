// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);

UCLASS()
class THEMAGICIAN_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;

	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButton;

	UFUNCTION()
	void InitializeSlot();
	
	UPROPERTY()
	FString LoadSlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;

	/** Field Notifies */
	void SetPlayerName(FString InPlayerName);
	void SetMapName(FString InMapName);

	FString GetPlayerName() const { return PlayerName; };
	FString GetMapName() const { return MapName; };

private:
	/** Field Notifies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess="true"))
	FString PlayerName = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess="true"))
	FString MapName = FString();
};
