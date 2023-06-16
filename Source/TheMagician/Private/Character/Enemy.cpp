// Copyright Fillipe Romero


#include "Character/Enemy.h"

#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/MagicianAbilitySystemLibrary.h"
#include "AbilitySystem/MagicianAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "TheMagician/TheMagician.h"
#include "UI/Widget/MagicianUserWidget.h"
#include "MagicianGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy::AEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	AbilitySystemComponent = CreateDefaultSubobject<UMagicianAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UMagicianAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
	
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AEnemy::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}

void AEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void AEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AEnemy::GetPlayerLevel()
{
	return Level;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	UMagicianAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent);

	if (UMagicianUserWidget* MagicianUserWidget = Cast<UMagicianUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		MagicianUserWidget->SetWidgetController(this);
	}

	// Broadcast when Health and maxHealth values change
	const UMagicianAttributeSet* MagicianAS = CastChecked<UMagicianAttributeSet>(AttributeSet);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MagicianAS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MagicianAS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	// Bind to GameplayTagEvent where if Effects_HitReact GameplayTag is removed or added, we'll call the callback function
	AbilitySystemComponent->RegisterGameplayTagEvent(FMagicianGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this,
		&ThisClass::HitReactTagChanged
	);

	// Broadcast Initial Values
	OnHealthChanged.Broadcast(MagicianAS->GetHealth());
	OnMaxHealthChanged.Broadcast(MagicianAS->GetMaxHealth());
}

void AEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void AEnemy::InitAbilityActorInfo()
{
	Super::InitAbilityActorInfo();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UMagicianAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttributes();
}

void AEnemy::InitializeDefaultAttributes() const
{
	UMagicianAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
