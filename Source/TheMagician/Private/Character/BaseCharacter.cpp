// Copyright Fillipe Romero

#include "Character/BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "MagicianGameplayTags.h"
#include "AbilitySystem/MagicianAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TheMagician/TheMagician.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = FMagicianGameplayTags::Get().Debuff_Burn;
	
	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->DebuffTag = FMagicianGameplayTags::Get().Debuff_Stun;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachComponent");
	EffectAttachComponent->SetupAttachment(GetRootComponent());

	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloOfProtectionNiagaraComponent");
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);

	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonNiagaraComponent");
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);

	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaSiphonNiagaraComponent");
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, bIsStunned);
	DOREPLIFETIME(ABaseCharacter, bIsBurned);
	DOREPLIFETIME(ABaseCharacter, bIsBeingShocked);
}

void ABaseCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void ABaseCharacter::BurnTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsBurned = NewCount > 0;
}

void ABaseCharacter::OnRep_Stunned()
{
}

void ABaseCharacter::OnRep_Burned()
{
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::InitAbilityActorInfo()
{
}
 
UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* ABaseCharacter::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void ABaseCharacter::Die(const FVector& DeathImpulse)
{
	// Detach will already be replicated
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	// Multicast to all Clients
	MulticastHandleDeath(DeathImpulse);
}

void ABaseCharacter::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	if (DeathSound) UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();
	bDead = true;
	BurnDebuffComponent->Deactivate();
	StunDebuffComponent->Deactivate();
	OnDeathDelegate.Broadcast(this);
}

void ABaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void ABaseCharacter::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);

	// Must be after Secondary Attributes
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ABaseCharacter::AddCharacterAbilities()
{
	// Should only happens in the Server
	if (!HasAuthority()) return;

	UMagicianAbilitySystemComponent* MagicianASC = CastChecked<UMagicianAbilitySystemComponent>(AbilitySystemComponent);

	MagicianASC->AddCharacterAbilities(StartupAbilities);
	MagicianASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

FVector ABaseCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& CombatSocketTag)
{
	const FMagicianGameplayTags& GameplayTags = FMagicianGameplayTags::Get();
	
	if (IsValid(Weapon) && CombatSocketTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon))
		return Weapon->GetSocketLocation(WeaponTipSocketName);

	if (CombatSocketTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
		return GetMesh()->GetSocketLocation(TailSocketName);

	if (CombatSocketTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
		return GetMesh()->GetSocketLocation(LeftHandSocketName);

	if (CombatSocketTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
		return GetMesh()->GetSocketLocation(RightHandSocketName);

	return FVector();
}

bool ABaseCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* ABaseCharacter::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> ABaseCharacter::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* ABaseCharacter::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage ABaseCharacter::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag.MatchesTagExact(MontageTag))
			return TaggedMontage;
	}
	
	return FTaggedMontage();
}

int32 ABaseCharacter::GetMinionCount_Implementation()
{
	return MinionCount;
}

void ABaseCharacter::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

ECharacterClass ABaseCharacter::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

USkeletalMeshComponent* ABaseCharacter::GetWeapon_Implementation()
{
	return Weapon;
}

bool ABaseCharacter::IsBeingShocked_Implementation() const
{
	return bIsBeingShocked;
}

void ABaseCharacter::SetIsBeingShocked_Implementation(bool bInShock)
{
	bIsBeingShocked = bInShock;
}

FOnASCRegistered& ABaseCharacter::GetOnASCRegisteredDelegate()
{
	return OnAscRegistered;
}

FOnDeathSignature& ABaseCharacter::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

void ABaseCharacter::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolveTimeline(DynamicMatInst);
	}

	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* WeaponDynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, WeaponDynamicMatInst);

		StartWeaponDissolveTimeline(WeaponDynamicMatInst);
	}
}
