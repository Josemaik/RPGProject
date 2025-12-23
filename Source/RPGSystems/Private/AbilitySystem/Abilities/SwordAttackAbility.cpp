// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/SwordAttackAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_KickTrace.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_SwordTrace.h"

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
	if (IsValid(WaitInputPressedEvent)) WaitInputPressedEvent->EndTask();
	if (IsValid(WaitStartComboEvent)) WaitStartComboEvent->EndTask();
	if (IsValid(WaitEndComboEvent)) WaitEndComboEvent->EndTask();
	if (IsValid(WaitEndHitTrace)) WaitEndHitTrace->EndTask();
	if (IsValid(WaitStartHitTrace)) WaitStartHitTrace->EndTask();
	if (IsValid(WaitKickHit)) WaitKickHit->EndTask();
	
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
	WaitKickHit->EventReceived.AddDynamic(this, &USwordAttackAbility::OnKickHitEventEnd);
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
	if (!IsValid(ShordTraceTask)) return;
	ShordTraceTask->Activate();
}

void USwordAttackAbility::OnHitEventEnd(FGameplayEventData Payload)
{
	ShordTraceTask->EndTask();
}

void USwordAttackAbility::OnKickHitEventEnd(FGameplayEventData Payload)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"KickTrace Activate");
	KickHitTrace = UAbilityTask_KickTrace::KickTrace(this,25.f);
	if (!IsValid(KickHitTrace)) return;
	KickHitTrace->Activate();
}

void USwordAttackAbility::OnInputPressed(float TimeWait)
{
	ManageComboLogic();
	SetupInputTask();
}

void USwordAttackAbility::ChooseAttack()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	FTimerManager& TimerManager = World->GetTimerManager();	
	
	FTimerHandle DelayBeforeAttackTimer;
	TimerManager.SetTimer(DelayBeforeAttackTimer,[this]()
	{
		switch (AttackIndex)
		{
			case 0: AnimInstance->Montage_Play(Attack1); AttackIndex = 1; 
				break;
			case 1: AnimInstance->Montage_Play(Attack2); AttackIndex = 2; 
				break;
			case 2: AnimInstance->Montage_Play(Attack3); AttackIndex = 3; 
				break;
			case 3: AnimInstance->Montage_Play(Attack4); AttackIndex = 4; 
				break;
			default: break;
		}
	},0.1f,false);
}

