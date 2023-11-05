// Copyright Fillipe Romero


#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "TheMagician/TheMagician.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);

	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	Super::Activate();

	// Called only Client Side
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		SendMouseCursorData();
	}
	else // Called only Server Side
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		// Bind function to the Delegate
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &ThisClass::OnTargetDataReplicatedCallback);
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

		// This means that the Delegate was not called yet and we need to wait for the data to come to the server (which means the stuffs inside SendMouseCursorData didn't reach the server yet)
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	// With this, basically we're saying that everything within this scope (code of this function) is being predicted
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (PC == nullptr) return;
	
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Target, false, CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	// Sends data to the Server
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey
	);
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		// Broadcast the Data to our custom Delegate
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// This tells the Ability System Component that the Target Data was received and it doesn't need to keep it stored anymore (cached)
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		// Broadcast the Data to our custom Delegate
		ValidData.Broadcast(DataHandle);
	}
}
