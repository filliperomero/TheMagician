// Copyright Fillipe Romero

#include "Actor/MagicCircle.h"
#include "Components/DecalComponent.h"

AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(GetRootComponent());
}

void AMagicCircle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
}
