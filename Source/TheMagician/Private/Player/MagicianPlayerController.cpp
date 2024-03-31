// Copyright Fillipe Romero


#include "Player/MagicianPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "MagicianGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Character/MainCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/MagicianInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "TheMagician/TheMagician.h"
#include "UI/Widget/DamageTextComponent.h"

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
	UpdateMagicCircleLocation();
}

void AMagicianPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (IsValid(MagicCircle)) return;
	
	MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);

	if (IsValid(DecalMaterial))
	{
		MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
	}
}

void AMagicianPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle)) MagicCircle->Destroy();
}

void AMagicianPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (!IsValid(TargetCharacter) || !DamageTextComponentClas || !IsLocalController()) return;

	UDamageTextComponent* DamageTextComponent = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClas);
	// Since we're creating one dynamic, we need to register it
	DamageTextComponent->RegisterComponent();
	// We're attaching it so we start off in the correct Location
	DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	// Since we just wanted the start location and we want the animation to just happen, we are detaching.
	DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	DamageTextComponent->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
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

void AMagicianPlayerController::UpdateMagicCircleLocation()
{
	if (!IsValid(MagicCircle)) return;

	MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
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
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FMagicianGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	// const FRotator Rotation = GetControlRotation();
	const FRotator Rotation = FRotator::ZeroRotator; // Since we have a fixed camera, we don't need to take in account the control rotation
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
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FMagicianGameplayTags::Get().Player_Block_CursorTrace))
	{
		if (LastHoveredActor) LastHoveredActor->UnHighlightActor();
		if (CurrentHoveredActor) CurrentHoveredActor->UnHighlightActor();
		LastHoveredActor = nullptr;
		CurrentHoveredActor = nullptr;
		return;
	}

	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);

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
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FMagicianGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	
	if (InputTag.MatchesTagExact(FMagicianGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = CurrentHoveredActor ? true : false;
		bAutoRunning = false;
	}
	
	if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
}

void AMagicianPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FMagicianGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	
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

			if (GetASC() && !GetASC()->HasMatchingGameplayTag(FMagicianGameplayTags::Get().Player_Block_InputPressed))
			{
				if (ClickNiagaraSystem)
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}
		}

		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AMagicianPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FMagicianGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}
	
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
