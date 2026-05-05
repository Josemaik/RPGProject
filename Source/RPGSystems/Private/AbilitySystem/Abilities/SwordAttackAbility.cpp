// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/SwordAttackAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "AbilitySystem/RPGAbilityTypes.h"
#include "AbilitySystem/NativeTags/RPGGameplayTags.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_KickTrace.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_SwordTrace.h"
#include "Kismet/GameplayStatics.h"
#include "Libraries/RPGAbilitySystemLibrary.h"

USwordAttackAbility::USwordAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AttackIndex = 0;
}

void USwordAttackAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void USwordAttackAbility::SetupInputTask()
{
	WaitInputPressedEvent = UAbilityTask_WaitInputPress::WaitInputPress(this,false);
	WaitInputPressedEvent->OnPress.AddDynamic(this,&USwordAttackAbility::OnInputPressed);
	WaitInputPressedEvent->ReadyForActivation();
}

void USwordAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	AnimInstance = ActorInfo->GetAnimInstance();
	
	SetupInputTask();
	
	SetupEventsTasks();
	
	ManageComboLogic();
}

void USwordAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DelayBeforeAttackTimer);
	}

	IsAttacking = false;
	saveAttack = false;
	AttackIndex = 0;
	//CanAttack = false;
	
	if (IsValid(WaitInputPressedEvent)) WaitInputPressedEvent->EndTask(); WaitInputPressedEvent = nullptr;
	if (IsValid(WaitStartComboEvent)) WaitStartComboEvent->EndTask(); WaitStartComboEvent = nullptr;
	if (IsValid(WaitEndComboEvent)) WaitEndComboEvent->EndTask(); WaitEndComboEvent = nullptr;
	if (IsValid(WaitEndHitTrace)) WaitEndHitTrace->EndTask(); WaitEndHitTrace = nullptr;
	if (IsValid(WaitStartHitTrace)) WaitStartHitTrace->EndTask(); WaitStartHitTrace = nullptr; 
	if (IsValid(WaitKickHit)) WaitKickHit->EndTask(); WaitKickHit = nullptr;
	if (IsValid(ShordTraceTask)) ShordTraceTask->EndTask(); ShordTraceTask = nullptr;
	if (IsValid(KickHitTrace)) KickHitTrace->EndTask(); KickHitTrace = nullptr;	
	
	Super::EndAbility(Handle,ActorInfo,ActivationInfo,bReplicateEndAbility, bWasCancelled);
}

void USwordAttackAbility::SetupEventsTasks()
{
	//Combo Events
	WaitStartComboEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,RPGGameplayTags::Combat::Events::Melee::StartCombo);
	WaitStartComboEvent->EventReceived.AddDynamic(this, &USwordAttackAbility::PlayCombo);
	WaitStartComboEvent->ReadyForActivation();

	WaitEndComboEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,RPGGameplayTags::Combat::Events::Melee::EndCombo);
	WaitEndComboEvent->EventReceived.AddDynamic(this, &USwordAttackAbility::StopCombo);
	WaitEndComboEvent->ReadyForActivation();

	//HitTrace Events
	WaitStartHitTrace = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,RPGGameplayTags::Combat::Events::Melee::HitStart);
	WaitStartHitTrace->EventReceived.AddDynamic(this, &USwordAttackAbility::OnHitEventStart);
	WaitStartHitTrace->ReadyForActivation();

	WaitEndHitTrace = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,RPGGameplayTags::Combat::Events::Melee::HitEnd);
	WaitEndHitTrace->EventReceived.AddDynamic(this, &USwordAttackAbility::OnHitEventEnd);
	WaitEndHitTrace->ReadyForActivation();

	WaitKickHit = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,RPGGameplayTags::Combat::Events::Actions::KickHit);
	WaitKickHit->EventReceived.AddDynamic(this, &USwordAttackAbility::OnKickHitEvent);
	WaitKickHit->ReadyForActivation();
}

void USwordAttackAbility::ManageComboLogic()
{
	if (CanAttack) 
	{
		if (IsAttacking)
		{
			saveAttack = true;
			return;
		}
		
		IsAttacking = true;
		ChooseAttack();
	}
}

void USwordAttackAbility::PlayCombo(FGameplayEventData Payload)
{
	if (!IsActive() || !CanAttack) return;
	if (saveAttack)
	{
		saveAttack = false;
		ChooseAttack();
		return;
	}
	//No input buffer - end combo
	StopCombo(Payload);
}

void USwordAttackAbility::StopCombo(FGameplayEventData Payload)
{
	IsAttacking = false;
	AttackIndex = 0;
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
}

void USwordAttackAbility::OnHitEventStart(FGameplayEventData Payload)
{
	ShordTraceTask = UAbilityTask_SwordTrace::ShordTrace(this,12.f);
	ShordTraceTask->OnHitActorDelegate.BindUObject(this,&USwordAttackAbility::OnHitActor);
	if (!IsValid(ShordTraceTask)) return;
	ShordTraceTask->Activate();
}

void USwordAttackAbility::OnHitActor(AActor* HitActor,const FVector& HitLocation)
{
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor))
	{
		FDamageEffectInfo DamageEffectInfo;
		CaptureDamageEffectInfo(HitActor, DamageEffectInfo);
		URPGAbilitySystemLibrary::ApplyDamageEffect(DamageEffectInfo);
		
		//GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Magenta,"Sword hit enemy");

		FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(OnHitEffect, DamageEffectInfo.AbilityLevel, ContextHandle);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		UGameplayStatics::PlayWorldCameraShake(GetWorld(),SwordHitCameraShakeClass,HitLocation,0.f,2000.f,1.f);
	}
}

void USwordAttackAbility::OnHitEventEnd(FGameplayEventData Payload)
{
	if (!IsValid(ShordTraceTask)) return;
	ShordTraceTask->EndTask();
}

void USwordAttackAbility::OnKickHitEvent(FGameplayEventData Payload)
{
	//GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"KickTrace Activate");
	KickHitTrace = UAbilityTask_KickTrace::KickTrace(this,25.f);
	KickHitTrace->OnHitActorDelegate.BindUObject(this,&USwordAttackAbility::OnHitActor);
	if (!IsValid(KickHitTrace)) return;
	KickHitTrace->Activate();
}

void USwordAttackAbility::OnInputPressed(float TimeWait)
{
	if (!IsActive()) return;
	
	ManageComboLogic();
	WaitInputPressedEvent->EndTask();
	SetupInputTask();
}

void USwordAttackAbility::ChooseAttack()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	TWeakObjectPtr<USwordAttackAbility> WeakThis(this);
	
	World->GetTimerManager().SetTimer(DelayBeforeAttackTimer,[WeakThis]()
	{
		if (!WeakThis.IsValid()) return;
		switch (WeakThis->AttackIndex)
		{
			case 0: WeakThis->AnimInstance->Montage_Play(WeakThis->Attack1); WeakThis->AttackIndex = 1; 
				break;
			case 1: WeakThis->AnimInstance->Montage_Play(WeakThis->Attack2); WeakThis->AttackIndex = 2; 
				break;
			case 2: WeakThis->AnimInstance->Montage_Play(WeakThis->Attack3); WeakThis->AttackIndex = 3; 
				break;
			case 3: WeakThis->AnimInstance->Montage_Play(WeakThis->Attack4); WeakThis->AttackIndex = 4; 
				break;
			default: break;
		}
	},0.1f,false);
}

