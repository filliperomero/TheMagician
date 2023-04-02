// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class THEMAGICIAN_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
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

	TObjectPtr<IEnemyInterface> LastHoveredActor;
	TObjectPtr<IEnemyInterface> CurrentHoveredActor;
	
};
