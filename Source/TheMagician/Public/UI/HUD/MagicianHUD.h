// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MagicianHUD.generated.h"

class UMagicianUserWidget;
/**
 * 
 */
UCLASS()
class THEMAGICIAN_API AMagicianHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UMagicianUserWidget> OverlayWidget;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMagicianUserWidget> OverlayWidgetClass;
	
};
