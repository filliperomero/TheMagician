// Copyright Fillipe Romero


#include "Actor/MagicianEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/MagicianAttributeSet.h"
#include "Components/SphereComponent.h"

AMagicianEffectActor::AMagicianEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
	
}

void AMagicianEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::EndOverlap);
}

void AMagicianEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO: Change this to apply a Gameplay Effect. For now, using const_cast as a hack!
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		// Very limited approach, we should use the Gameplay Effects. To be implemented later
		const UMagicianAttributeSet* MagicianAttributeSet = Cast<UMagicianAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UMagicianAttributeSet::StaticClass()));

		// Don't do that! This is for learning purpose only
		UMagicianAttributeSet* MutableMagicianAttributeSet = const_cast<UMagicianAttributeSet*>(MagicianAttributeSet);
		MutableMagicianAttributeSet->SetHealth(MagicianAttributeSet->GetHealth() + 20.f);

		Destroy();
	}
}

void AMagicianEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
