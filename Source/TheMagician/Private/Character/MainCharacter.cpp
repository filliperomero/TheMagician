// Copyright Fillipe Romero


#include "Character/MainCharacter.h"

#include "AbilitySystemComponent.h"
#include "MagicianGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
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
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("Follow Camera");
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	CharacterClass = ECharacterClass::Elementalist;

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LevelUpNiagaraComponent"));
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;
}

void AMainCharacter::OnRep_Stunned()
{
	Super::OnRep_Stunned();

	if (UMagicianAbilitySystemComponent* MagicianASC = Cast<UMagicianAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FMagicianGameplayTags GameplayTags = FMagicianGameplayTags::Get();
		
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);

		if (bIsStunned)
		{
			MagicianASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
		}
		else
		{
			MagicianASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
	}
}

void AMainCharacter::OnRep_Burned()
{
	Super::OnRep_Burned();

	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

void AMainCharacter::InitAbilityActorInfo()
{
	AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	MagicianPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(MagicianPlayerState, this);
	Cast<UMagicianAbilitySystemComponent>(MagicianPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	AbilitySystemComponent = MagicianPlayerState->GetAbilitySystemComponent();
	AttributeSet = MagicianPlayerState->GetAttributeSet();
	OnAscRegistered.Broadcast(AbilitySystemComponent);
	
	AbilitySystemComponent->RegisterGameplayTagEvent(FMagicianGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AMainCharacter::StunTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(FMagicianGameplayTags::Get().Debuff_Burn, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AMainCharacter::BurnTagChanged);

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
	MulticastLevelUpParticles();
}

void AMainCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (!IsValid(LevelUpNiagaraComponent)) return;

	const FVector CameraLocation = FollowCamera->GetComponentLocation();
	const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
	const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();

	LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
	LevelUpNiagaraComponent->Activate(true);
}

void AMainCharacter::AddSpellPoints_Implementation(int32 InSpellPoints)
{
	AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	MagicianPlayerState->AddSpellPoints(InSpellPoints);
}

void AMainCharacter::AddAttributePoints_Implementation(int32 InAttributePoints)
{
	AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	MagicianPlayerState->AddAttributePoints(InAttributePoints);
}

void AMainCharacter::AddPlayerLevel_Implementation(int32 InLevel)
{
	AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	MagicianPlayerState->AddToLevel(InLevel);

	if (UMagicianAbilitySystemComponent* MagicianASC = Cast<UMagicianAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		MagicianASC->UpdateAbilityStatuses(MagicianPlayerState->GetPlayerLevel());
	}
}

int32 AMainCharacter::GetAttributePoints_Implementation() const
{
	const AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	return MagicianPlayerState->GetAttributePoints();
}

int32 AMainCharacter::GetSpellPoints_Implementation() const
{
	const AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	return MagicianPlayerState->GetSpellPoints();
}

void AMainCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AMagicianPlayerController* MagicianPlayerController = Cast<AMagicianPlayerController>(GetController()))
	{
		MagicianPlayerController->ShowMagicCircle(DecalMaterial);
		MagicianPlayerController->bShowMouseCursor = false;
	}
}

void AMainCharacter::HideMagicCircle_Implementation()
{
	if (AMagicianPlayerController* MagicianPlayerController = Cast<AMagicianPlayerController>(GetController()))
	{
		MagicianPlayerController->HideMagicCircle();
		MagicianPlayerController->bShowMouseCursor = true;
	}
}

int32 AMainCharacter::GetPlayerLevel_Implementation()
{
	const AMagicianPlayerState* MagicianPlayerState = GetPlayerState<AMagicianPlayerState>();
	check(MagicianPlayerState);

	return MagicianPlayerState->GetPlayerLevel();
}
