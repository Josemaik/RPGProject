// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/CrossbowAttackAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/NativeTags/RPGGameplayTags.h"

void UCrossbowAttackAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UCrossbowAttackAbility::SetupInputTask()
{
	// WaitInputPressedEvent = UAbilityTask_WaitInputPress::WaitInputPress(this,false);
	// WaitInputPressedEvent->OnPress.AddDynamic(this,&UCrossbowAttackAbility::OnInputPressed);
	// WaitInputPressedEvent->ReadyForActivation();
	OnInputPressed(0.f);
	
	WaitInputReleasedEvent = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	WaitInputReleasedEvent->OnRelease.AddDynamic(this,&UCrossbowAttackAbility::OnInputReleased);
	WaitInputReleasedEvent->ReadyForActivation();
}

void UCrossbowAttackAbility::ReadyToShoot(FGameplayEventData Payload)
{
	bIsReadyToShoot = true;
	AnimInstance->Montage_Pause(AimCrossbowMontage);

	if (bInputReleasedBeforeReady)
	{
		OnInputReleased(0.f);
	}
}

void UCrossbowAttackAbility::OnInputPressed(float TimeWaited)
{
	if (!IsValid(AnimInstance) || !IsValid(AimCrossbowMontage))
	{
		return;
	}
	
	bIsAiming = true;
	
	AnimInstance->Montage_Play(AimCrossbowMontage,-1.f,
		EMontagePlayReturnType::MontageLength, AimCrossbowMontage->GetPlayLength());

	WaitAimReady = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,RPGGameplayTags::Combat::Events::Ranged::CrossbowReady);
	WaitAimReady->EventReceived.AddDynamic(this, &UCrossbowAttackAbility::ReadyToShoot);
	WaitAimReady->ReadyForActivation();
}

void UCrossbowAttackAbility::OnInputReleased(float TimeWaited)
{
	if (!bIsReadyToShoot)
	{
		bInputReleasedBeforeReady = true;
		return;
	}
	
	if (bIsAiming)
	{
		//Shoot();
		//Init timer and sheatcrossbow
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UCrossbowAttackAbility::SheathCrossbow);
		GetWorld()->GetTimerManager().SetTimer(WaitUntilSheathTimer,TimerDelegate,3.f,false);
		
		return;
	}
	//Cancelled
	SheathCrossbow();
}

void UCrossbowAttackAbility::SheathCrossbow()
{
	bIsAiming = false;
	
	AnimInstance->Montage_SetPlayRate(AimCrossbowMontage, 1.f);
	AnimInstance->Montage_Resume(AimCrossbowMontage);

	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
}

void UCrossbowAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AnimInstance = ActorInfo->GetAnimInstance();
	SetupInputTask();
}

void UCrossbowAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(WaitInputReleasedEvent)) { WaitInputReleasedEvent->EndTask(); WaitInputReleasedEvent = nullptr; }

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaitUntilSheathTimer);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UCrossbowAttackAbility::UCrossbowAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}


