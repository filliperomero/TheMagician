// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "MainCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
/**
 * 
 */
UCLASS()
class THEMAGICIAN_API AMainCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

protected:

private:
	UPROPERTY(EditAnywhere, Category = Camera)
	float MaxCameraBoomDistance = 750.f;
	
	UPROPERTY(EditAnywhere, Category = Camera)
	float MinCameraBoomDistance = 300.f;
	
	/* Components */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE float GetMaxCameraBoomDistance() const { return MaxCameraBoomDistance; }
	FORCEINLINE float GetMinCameraBoomDistance() const { return MinCameraBoomDistance; }
};
