// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Enemy.generated.h"

class AMagicianAIController;
class UBehaviorTree;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class THEMAGICIAN_API AEnemy : public ABaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void PossessedBy(AController* NewController) override;

	virtual void Die(const FVector& DeathImpulse) override;

	/** Begin Enemy Interface */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** End Enemy Interface */

	/** Begin Combat Interface */
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	/** End Combat Interface */

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	UPROPERTY(BlueprintReadOnly, Category = Combat)
	bool bHitReacting { false };
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float LifeSpan { 5.f };

	UPROPERTY(BlueprintReadWrite, Category = Combat)
	TObjectPtr<AActor> CombatTarget;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level { 1 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = AI)
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AMagicianAIController> MagicianAIController;
};
