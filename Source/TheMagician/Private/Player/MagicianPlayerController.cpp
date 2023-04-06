// Copyright Fillipe Romero


#include "Player/MagicianPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interaction/EnemyInterface.h"

AMagicianPlayerController::AMagicianPlayerController()
{
	bReplicates = true;
}

void AMagicianPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AMagicianPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(InputContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);

		bShowMouseCursor = true;
		DefaultMouseCursor = EMouseCursor::Default;

		FInputModeGameAndUI InputModeData;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(false);
		
		SetInputMode(InputModeData);
	}
}

void AMagicianPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(MoveCameraAction, ETriggerEvent::Triggered, this, &ThisClass::MoveCamera);
}

void AMagicianPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AMagicianPlayerController::MoveCamera(const FInputActionValue& InputActionValue)
{
	const float Value = InputActionValue.Get<float>();

	if (AMainCharacter* MainCharacter = GetPawn<AMainCharacter>())
	{
		if (MainCharacter->GetCameraBoom())
		{
			const float CurrentCameraValue = MainCharacter->GetCameraBoom()->TargetArmLength;
			const float NewTargetValue = FMath::Clamp(CurrentCameraValue + CameraZoomRate * Value, MainCharacter->GetMinCameraBoomDistance(), MainCharacter->GetMaxCameraBoomDistance());

			MainCharacter->GetCameraBoom()->TargetArmLength = NewTargetValue;
		}
	}
}

void AMagicianPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// Didn't hit anything
	if (!CursorHit.bBlockingHit) return;

	LastHoveredActor = CurrentHoveredActor;
	CurrentHoveredActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	/**
	 * Line Trace from cursor. There are several Scenarios:
	 *	A. LastActor is null && ThisActor is null
	 *		- Do nothing
	 *	B. LastActor is null && ThisActor is valid
	 *		- Highlight ThisActor
	 *	C. LastActor is valid && ThisActor is null
	 *		- UnHighlight LastActor
	 *	D. Both actors are valid, but LastActor != ThisActor
	 *		- UnHighlight LastActor and Highlight ThisActor
	 *	E. Both actors are valid, and are the same actor
	 *		- Do nothing
	 */

	if (LastHoveredActor == nullptr)
	{
		if (CurrentHoveredActor != nullptr) CurrentHoveredActor->HighlightActor();
	}
	else
	{
		if (CurrentHoveredActor == nullptr)
		{
			LastHoveredActor->UnHighlightActor();
		}
		else if (LastHoveredActor != CurrentHoveredActor)
		{
			LastHoveredActor->UnHighlightActor();
			CurrentHoveredActor->HighlightActor();
		}
	}
}
