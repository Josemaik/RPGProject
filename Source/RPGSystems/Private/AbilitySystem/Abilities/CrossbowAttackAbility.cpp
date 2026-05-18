// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/CrossbowAttackAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/NativeTags/RPGGameplayTags.h"
#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Character/RPGSystemsCharacter.h"
#include "Character/Animation/RPGAnimInstance.h"

void UCrossbowAttackAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UCrossbowAttackAbility::SetupInputTask()
{
	WaitInputReleasedEvent = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	WaitInputReleasedEvent->OnRelease.AddDynamic(this,&UCrossbowAttackAbility::OnInputReleased);
	WaitInputReleasedEvent->ReadyForActivation();
}

void UCrossbowAttackAbility::SetupMontageEvents()
{
	WaitAimReady = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,RPGGameplayTags::Combat::Events::Ranged::CrossbowReady);
	WaitAimReady->EventReceived.AddDynamic(this, &UCrossbowAttackAbility::ReadyToShoot);
	WaitAimReady->ReadyForActivation();
}

void UCrossbowAttackAbility::ReadyToShoot(FGameplayEventData Payload)
{
	bIsReadyToShoot = true;
	//AnimInstance->Montage_Pause(AimCrossbowMontage);
}

void UCrossbowAttackAbility::OnInputReleased(float TimeWaited)
{
	if (bIsReadyToShoot)
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
	bIsReadyToShoot = false;
	AnimInstance->SetIsCrossbowAiming(false);
	// AnimInstance->Montage_SetPlayRate(AimCrossbowMontage, 1.f);
	// AnimInstance->Montage_Resume(AimCrossbowMontage);
	OwnerCharacter->OnRangeStopAiming();
	OwnerCharacter->ChangueEquipmentAttachPoint(RPGInventoryTags::AttachPoint::LeftHand,RPGInventoryTags::AttachPoint::Back);
	
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
}

void UCrossbowAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AnimInstance = Cast<URPGAnimInstance>(ActorInfo->GetAnimInstance());
	OwnerCharacter = Cast<ARPGSystemsCharacter>(ActorInfo->AvatarActor);
	
	SetupInputTask(); //release task

	if (!IsValid(AnimInstance) || !IsValid(OwnerCharacter)/* || !IsValid(AimCrossbowMontage)*/)
	{
		return;
	}

	AnimInstance->SetIsCrossbowAiming(true);
	OwnerCharacter->OnRangeStartAiming();
	OwnerCharacter->ChangueEquipmentAttachPoint(RPGInventoryTags::AttachPoint::Back,RPGInventoryTags::AttachPoint::LeftHand);
	//play montage
	// AnimInstance->Montage_Play(AimCrossbowMontage,-1.f,
	// 	EMontagePlayReturnType::MontageLength, AimCrossbowMontage->GetPlayLength());
	
	//SetupMontageEvents();
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


