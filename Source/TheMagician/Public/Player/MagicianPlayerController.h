// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "MagicianPlayerController.generated.h"

class UMagicianAbilitySystemComponent;
class UMagicianInputConfig;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class THEMAGICIAN_API AMagicianPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMagicianPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UMagicianAbilitySystemComponent* GetASC();
	
	UPROPERTY()
	TObjectPtr<UMagicianAbilitySystemComponent> MagicianAbilitySystemComponent;
	
	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraZoomRate = 40.f;
	
	/* Enhanced Input */
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MoveCameraAction;

	/* Callbacks for Input */
	void Move(const FInputActionValue& InputActionValue);
	void MoveCamera(const FInputActionValue& InputActionValue);

	void CursorTrace();

	IEnemyInterface* LastHoveredActor;
	IEnemyInterface* CurrentHoveredActor;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UMagicianInputConfig> InputConfig;
	
};
