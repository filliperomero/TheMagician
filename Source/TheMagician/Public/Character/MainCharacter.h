// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interaction/PlayerInterface.h"
#include "MainCharacter.generated.h"

class UNiagaraComponent;
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
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual void AddSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual void AddAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual void AddPlayerLevel_Implementation(int32 InLevel) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	/** End Player Interface */

	/** Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	/** End Combat Interface */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	virtual void OnRep_Stunned() override;
	virtual void OnRep_Burned() override;

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

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE float GetMaxCameraBoomDistance() const { return MaxCameraBoomDistance; }
	FORCEINLINE float GetMinCameraBoomDistance() const { return MinCameraBoomDistance; }
};
