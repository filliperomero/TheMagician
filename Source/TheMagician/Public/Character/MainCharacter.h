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
	/* Components */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;
	
};
