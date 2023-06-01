// Copyright Fillipe Romero


#include "Character/MainCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/MagicianPlayerController.h"
#include "Player/MagicianPlayerState.h"
#include "UI/HUD/MagicianHUD.h"

AMainCharacter::AMainCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = MaxCameraBoomDistance;
	CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("Follow Camera");
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;
}

void AMainCharacter::InitAbilityActorInfo()
{
	AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	MagicianPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(MagicianPlayerState, this);
	Cast<UMagicianAbilitySystemComponent>(MagicianPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	AbilitySystemComponent = MagicianPlayerState->GetAbilitySystemComponent();
	AttributeSet = MagicianPlayerState->GetAttributeSet();

	// In multiplayer, the PlayerController will be invalid in other people computers, since I can only get my PlayerController but not others
	if (AMagicianPlayerController* MagicianPlayerController = Cast<AMagicianPlayerController>(GetController()))
	{
		// The HUD is also only valid to locally controller player
		if (AMagicianHUD* MagicianHUD = Cast<AMagicianHUD>(MagicianPlayerController->GetHUD()))
		{
			MagicianHUD->InitOverlay(MagicianPlayerController, MagicianPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}

	InitializeDefaultAttributes();
}

void AMainCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability Actor Info for the Server
	InitAbilityActorInfo();

	AddCharacterAbilities();
}

void AMainCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability Actor Info for the Client
	InitAbilityActorInfo();
}

int32 AMainCharacter::GetPlayerLevel()
{
	const AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	return MagicianPlayerState->GetPlayerLevel();
}
