// Copyright Fillipe Romero


#include "Player/MagicianPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "MagicianGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "Character/MainCharacter.h"
#include "Components/SplineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/MagicianInputComponent.h"
#include "Interaction/EnemyInterface.h"

AMagicianPlayerController::AMagicianPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AMagicianPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	if (bAutoRunning) AutoRun();
}

void AMagicianPlayerController::AutoRun()
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);

		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunningAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AMagicianPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(InputContext);

	// We should not check() here since we only going to get the Subsystem when we are locally controlled
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);		
	}
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
		
	SetInputMode(InputModeData);
}

void AMagicianPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UMagicianInputComponent* MagicianInputComponent = CastChecked<UMagicianInputComponent>(InputComponent);
	
	MagicianInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	MagicianInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ThisClass::ShiftPressed);
	MagicianInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ThisClass::ShiftReleased);
	MagicianInputComponent->BindAction(MoveCameraAction, ETriggerEvent::Triggered, this, &ThisClass::MoveCamera);

	MagicianInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

UMagicianAbilitySystemComponent* AMagicianPlayerController::GetASC()
{
	if (MagicianAbilitySystemComponent == nullptr)
		MagicianAbilitySystemComponent = Cast<UMagicianAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));

	return MagicianAbilitySystemComponent;
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
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// Didn't hit anything
	if (!CursorHit.bBlockingHit) return;

	LastHoveredActor = CurrentHoveredActor;
	CurrentHoveredActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (LastHoveredActor != CurrentHoveredActor)
	{
		if (LastHoveredActor) LastHoveredActor->UnHighlightActor();
		if (CurrentHoveredActor) CurrentHoveredActor->HighlightActor();
	}
}

void AMagicianPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FMagicianGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = CurrentHoveredActor ? true : false;
		bAutoRunning = false;
	}
}

void AMagicianPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FMagicianGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);

		return;
	}
	
	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	
	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// Create a Navigation Path
			UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination);
			if (NavPath)
			{
				Spline->ClearSplinePoints();
				if (!NavPath->PathPoints.IsEmpty())
				{
					for (const FVector& PointLoc : NavPath->PathPoints)
					{
						Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					}
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
					bAutoRunning = true;
				}
			}
		}

		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AMagicianPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FMagicianGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);

		return;
	}

	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}
