// Copyright Fillipe Romero


#include "AbilitySystem/MagicianAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Player/MagicianPlayerState.h"
#include "UI/HUD/MagicianHUD.h"
#include "UI/WidgetController/MagicianWidgetController.h"

UOverlayWidgetController* UMagicianAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if(AMagicianHUD* MagicianHUD = Cast<AMagicianHUD>(PC->GetHUD()))
		{
			AMagicianPlayerState* PS = PC->GetPlayerState<AMagicianPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);

			return MagicianHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}

	return nullptr;
}
