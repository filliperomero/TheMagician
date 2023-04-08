// Copyright Fillipe Romero

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicianEffectActor.generated.h"

class USphereComponent;

UCLASS()
class THEMAGICIAN_API AMagicianEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMagicianEffectActor();

	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

};
