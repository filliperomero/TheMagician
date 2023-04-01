// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

// Putting Abstract here will disable the ability to create BP from it
UCLASS(Abstract)
class THEMAGICIAN_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<USkeletalMeshComponent> Weapon; 

};
