// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

class THEMAGICIAN_API IPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
	
	UFUNCTION(BlueprintNativeEvent)
	void AddPlayerLevel(int32 InLevel);
	
	UFUNCTION(BlueprintNativeEvent)
	void AddAttributePoints(int32 InAttributePoints);

	UFUNCTION(BlueprintNativeEvent)
	void AddSpellPoints(int32 InSpellPoints);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();
	
};
