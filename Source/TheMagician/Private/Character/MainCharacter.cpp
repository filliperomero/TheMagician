// Copyright Fillipe Romero


#include "Character/MainCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
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

	CharacterClass = ECharacterClass::Elementalist;
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

int32 AMainCharacter::GetXP_Implementation() const
{
	const AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	return MagicianPlayerState->GetXP();
}

int32 AMainCharacter::FindLevelForXP_Implementation(int32 InXP)
{
	const AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	return MagicianPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AMainCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	const AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	return MagicianPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 AMainCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	const AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	return MagicianPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

void AMainCharacter::AddToXP_Implementation(int32 InXP)
{
	AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	return MagicianPlayerState->AddToXP(InXP);
}

void AMainCharacter::LevelUp_Implementation()
{
	
}

void AMainCharacter::AddSpellPoints_Implementation(int32 InSpellPoints)
{
	// Todo: Add Spell Points to PlayerState
}

void AMainCharacter::AddAttributePoints_Implementation(int32 InAttributePoints)
{
	// Todo: Add Attribute Points to PlayerState
}

void AMainCharacter::AddPlayerLevel_Implementation(int32 InLevel)
{
	AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	MagicianPlayerState->AddToLevel(InLevel);
}

int32 AMainCharacter::GetPlayerLevel_Implementation()
{
	const AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	return MagicianPlayerState->GetPlayerLevel();
}
