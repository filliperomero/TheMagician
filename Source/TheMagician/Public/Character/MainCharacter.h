// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interaction/PlayerInterface.h"
#include "MainCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
/**
 * 
 */
UCLASS()
class THEMAGICIAN_API AMainCharacter : public ABaseCharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AMainCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/** Player Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	/** End Player Interface */

	/** Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	/** End Combat Interface */

protected:
	virtual void InitAbilityActorInfo() override;

private:
	
	UPROPERTY(EditAnywhere, Category = Camera)
	float MaxCameraBoomDistance = 1200.f; // Go back to 750.f later
	
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
