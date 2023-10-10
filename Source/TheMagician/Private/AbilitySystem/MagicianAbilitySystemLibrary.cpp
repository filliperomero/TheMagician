// Copyright Fillipe Romero


#include "AbilitySystem/MagicianAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MagicianAbilityTypes.h"
#include "MagicianGameplayTags.h"
#include "Game/MagicianGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MagicianPlayerState.h"
#include "UI/HUD/MagicianHUD.h"
#include "UI/WidgetController/MagicianWidgetController.h"

bool UMagicianAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AMagicianHUD*& OutMagicianHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutMagicianHUD = Cast<AMagicianHUD>(PC->GetHUD());
		if(OutMagicianHUD)
		{
			AMagicianPlayerState* PS = PC->GetPlayerState<AMagicianPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			
			return true;
		}
	}

	return false;
}

UOverlayWidgetController* UMagicianAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AMagicianHUD* MagicianHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, MagicianHUD))
	{
		return  MagicianHUD->GetOverlayWidgetController(WCParams);
	}
	
	return nullptr;
}

UAttributeMenuWidgetController* UMagicianAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AMagicianHUD* MagicianHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, MagicianHUD))
	{
		return  MagicianHUD->GetAttributeMenuWidgetController(WCParams);
	}
	
	return nullptr;
}

USpellMenuWidgetController* UMagicianAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AMagicianHUD* MagicianHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, MagicianHUD))
	{
		return  MagicianHUD->GetSpellMenuWidgetController(WCParams);
	}
	
	return nullptr;
}

void UMagicianAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	
	const AActor* AvatarActor = ASC->GetAvatarActor();

	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UMagicianAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;

	for (const TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	int32 AbilityLevel = 1;

	if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		AbilityLevel = ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor());

	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, AbilityLevel);
		ASC->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UMagicianAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AMagicianGameModeBase* MagicianGameMode = Cast<AMagicianGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));

	if (MagicianGameMode == nullptr) return nullptr;

	return MagicianGameMode->CharacterClassInfo;
}

UAbilityInfo* UMagicianAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AMagicianGameModeBase* MagicianGameMode = Cast<AMagicianGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));

	if (MagicianGameMode == nullptr) return nullptr;

	return MagicianGameMode->AbilityInfo;
}

bool UMagicianAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMagicianGameplayEffectContext* MagicianContext = static_cast<const FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		return MagicianContext->IsBlockedHit();

	return false;
}

void UMagicianAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FMagicianGameplayEffectContext* MagicianContext = static_cast<FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		MagicianContext->SetIsBlockedHit(bInIsBlockedHit);
}

bool UMagicianAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMagicianGameplayEffectContext* MagicianContext = static_cast<const FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		return MagicianContext->IsCriticalHit();

	return false;
}

void UMagicianAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FMagicianGameplayEffectContext* MagicianContext = static_cast<FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		MagicianContext->SetIsCriticalHit(bInIsCriticalHit);
}

bool UMagicianAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMagicianGameplayEffectContext* MagicianContext = static_cast<const FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		return MagicianContext->IsSuccessfulDebuff();

	return false;
}

void UMagicianAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsDebuff)
{
	if (FMagicianGameplayEffectContext* MagicianContext = static_cast<FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		MagicianContext->SetIsSuccessfulDebuff(bInIsDebuff);
}

float UMagicianAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMagicianGameplayEffectContext* MagicianContext = static_cast<const FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		return MagicianContext->GetDebuffDamage();

	return 0.f;
}

void UMagicianAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FMagicianGameplayEffectContext* MagicianContext = static_cast<FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		MagicianContext->SetDebuffDamage(InDamage);
}

float UMagicianAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMagicianGameplayEffectContext* MagicianContext = static_cast<const FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		return MagicianContext->GetDebuffDuration();

	return 0.f;
}

void UMagicianAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FMagicianGameplayEffectContext* MagicianContext = static_cast<FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		MagicianContext->SetDebuffDuration(InDuration);
}

float UMagicianAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMagicianGameplayEffectContext* MagicianContext = static_cast<const FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		return MagicianContext->GetDebuffFrequency();

	return 0.f;
}

void UMagicianAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FMagicianGameplayEffectContext* MagicianContext = static_cast<FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
		MagicianContext->SetDebuffFrequency(InFrequency);
}

FGameplayTag UMagicianAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMagicianGameplayEffectContext* MagicianContext = static_cast<const FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (MagicianContext->GetDamageType().IsValid())
		{
			return *MagicianContext->GetDamageType();
		}
	}

	return FGameplayTag();
}

void UMagicianAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FMagicianGameplayEffectContext* MagicianContext = static_cast<FMagicianGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		MagicianContext->SetDamageType(DamageType);
		
	}
}

void UMagicianAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);

		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

bool UMagicianAbilitySystemLibrary::IsNotFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;

	return !bFriends;
}

FGameplayEffectContextHandle UMagicianAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FMagicianGameplayTags& GameplayTags = FMagicianGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Info_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Info_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Info_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Info_Frequency, DamageEffectParams.DebuffFrequency);
	
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	return EffectContextHandle;
}

int32 UMagicianAbilitySystemLibrary::GetXPRewardByClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	const FCharacterClassDefaultInfo Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	return static_cast<int32>(Info.XPReward.GetValueAtLevel(CharacterLevel));
}
