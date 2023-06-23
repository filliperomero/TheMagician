// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "MagicianPlayerController.generated.h"

class UDamageTextComponent;
class UMagicianAbilitySystemComponent;
class UMagicianInputConfig;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
class USplineComponent;
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

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

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
	TObjectPtr<UInputAction> ShiftAction;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MoveCameraAction;

	/* Callbacks for Input */
	void Move(const FInputActionValue& InputActionValue);
	FORCEINLINE void ShiftPressed() { bShiftKeyDown = true; };
	FORCEINLINE void ShiftReleased() { bShiftKeyDown = false; };;
	void MoveCamera(const FInputActionValue& InputActionValue);

	bool bShiftKeyDown = false;

	FHitResult CursorHit;
	void CursorTrace();

	IEnemyInterface* LastHoveredActor;
	IEnemyInterface* CurrentHoveredActor;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UMagicianInputConfig> InputConfig;

	/** Click To Move */
	void AutoRun();
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime { 0.f };
	float ShortPressThreshold { 0.5f };
	bool bAutoRunning = false;
	bool bTargeting = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Click to Move Properties")
	float AutoRunningAcceptanceRadius { 50.f };

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClas;
	
};
