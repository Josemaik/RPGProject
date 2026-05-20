// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/CrossbowAttackAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_ArrowProjectileHits.h"
#include "AbilitySystem/NativeTags/RPGGameplayTags.h"
#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Character/RPGSystemsCharacter.h"
#include "Character/Animation/RPGAnimInstance.h"
#include "Components/ArrowComponent.h"
#include "Equipment/EquipmentActors/ArrowActor.h"
#include "Equipment/EquipmentActors/CrossBowActor.h"

void UCrossbowAttackAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UCrossbowAttackAbility::SetupInputTask()
{
	WaitInputReleasedEvent = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	WaitInputReleasedEvent->OnRelease.AddDynamic(this,&UCrossbowAttackAbility::OnInputReleased);
	WaitInputReleasedEvent->ReadyForActivation();

	//if input is pressed again before sheat crossbot timer finishes -> reload montage and clear timer
	WaitInputPressedEvent = UAbilityTask_WaitInputPress::WaitInputPress(this, false);
	WaitInputPressedEvent->OnPress.AddDynamic(this,&UCrossbowAttackAbility::OnInputPressed);
	WaitInputPressedEvent->ReadyForActivation();
}

// void UCrossbowAttackAbility::SetupMontageEvents()
// {
// 	WaitAimReady = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,RPGGameplayTags::Combat::Events::Ranged::CrossbowReady);
// 	WaitAimReady->EventReceived.AddDynamic(this, &UCrossbowAttackAbility::ReadyToShoot);
// 	WaitAimReady->ReadyForActivation();
// }
//
// void UCrossbowAttackAbility::ReadyToShoot(FGameplayEventData Payload)
// {
// 	bIsReadyToShoot = true;
// 	//AnimInstance->Montage_Pause(AimCrossbowMontage);
// }

void UCrossbowAttackAbility::OnInputPressed(float TimeWaited)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Blue,"Input pressed");
	
	bInputPressedInCalcelWindow = true;

	WaitInputPressedEvent = UAbilityTask_WaitInputPress::WaitInputPress(this, false);
	WaitInputPressedEvent->OnPress.AddDynamic(this, &UCrossbowAttackAbility::OnInputPressed);
	WaitInputPressedEvent->ReadyForActivation();
}

void UCrossbowAttackAbility::OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		return;
	}
	
	if (IsValid(Montage) && Montage == ReloadCrossbowMontage)
	{
		CrossBowActor->AttachArrow();
		
		StartCancelWindow();
	}
}

void UCrossbowAttackAbility::StartCancelWindow()
{
	bIsCancelTimeActive = true;
	//Init timer and sheatcrossbow
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UCrossbowAttackAbility::OnCancelWindowFinished);
	GetWorld()->GetTimerManager().SetTimer(WaitUntilSheathTimer,TimerDelegate,2.f,false);
}

void UCrossbowAttackAbility::Reload()
{
	if (bIsCancelTimeActive)
	{
		return;
	}

	SpawnArrow();
	
	float Duration = AnimInstance->Montage_Play(ReloadCrossbowMontage,1.2f,
	EMontagePlayReturnType::Duration,0.f);
}

void UCrossbowAttackAbility::OnInputReleased(float TimeWaited)
{
	if (bIsCancelTimeActive)
	{
		return;
	}
	
	ShootArrow();
	
	Reload();
}

void UCrossbowAttackAbility::ShootArrow()
{
	if (!IsValid(CrossBowActor) || bIsCancelTimeActive)
	{
		return;
	}
	
	CrossBowActor->Shoot();
}

void UCrossbowAttackAbility::SpawnArrow()
{
	FDamageEffectInfo DamageEffectInfo;
	CaptureDamageEffectInfo(nullptr, DamageEffectInfo);
	
	CrossBowActor->SpawnArrow(DamageEffectInfo);
}

void UCrossbowAttackAbility::OnCancelWindowFinished()
{
	bIsCancelTimeActive = false;
	
	if (bInputPressedInCalcelWindow)
	{
		bInputPressedInCalcelWindow = false;
		OnInputReleased(0.f);
		return;
	}
	
	AnimInstance->SetIsCrossbowAiming(false);
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
	
	SetupInputTask(); //release and press

	if (!IsValid(AnimInstance) || !IsValid(OwnerCharacter))
	{
		return;
	}

	//set animation state
	AnimInstance->SetIsCrossbowAiming(true);
	AnimInstance->OnMontageEnded.AddDynamic(this,&UCrossbowAttackAbility::OnReloadMontageEnded);

	//camera and movement
	OwnerCharacter->OnRangeStartAiming();
	//mesh attach
	OwnerCharacter->ChangueEquipmentAttachPoint(RPGInventoryTags::AttachPoint::Back,RPGInventoryTags::AttachPoint::LeftHand);

	//cross bow data
	CrossBowActor = Cast<ACrossBowActor>(OwnerCharacter->GetEquipmentActor(RPGInventoryTags::AttachPoint::LeftHand));
	if (!IsValid(CrossBowActor))
	{
		return;
	}
	
	CrossBowActor->SetOwner(OwnerCharacter);

	if (!CrossBowActor->bHasArrow)
	{
		SpawnArrow();
		CrossBowActor->AttachArrow();
	}
}

void UCrossbowAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(WaitInputReleasedEvent)) { WaitInputReleasedEvent->EndTask(); WaitInputReleasedEvent = nullptr; }
	if (IsValid(WaitInputPressedEvent)) { WaitInputPressedEvent->EndTask(); WaitInputPressedEvent = nullptr; }
	if (IsValid(ArrowProjectileTask)) { ArrowProjectileTask->EndTask(); ArrowProjectileTask = nullptr; }

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaitUntilSheathTimer);
		World->GetTimerManager().ClearTimer(WaitAfterShootTimer);
	}

	AnimInstance->OnMontageEnded.RemoveAll(this);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UCrossbowAttackAbility::UCrossbowAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}


