// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/SwordAttackAbility.h"

#include "Character/RPGSystemsCharacter.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_SwordTrace.h"

USwordAttackAbility::USwordAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AttackIndex = 0;
}

void USwordAttackAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"Give SwordAttackAbility");
}

void USwordAttackAbility::SetupInput()
{
	WaitInputPressedEvent = UAbilityTask_WaitInputPress::WaitInputPress(this,false);
	WaitInputPressedEvent->OnPress.AddDynamic(this,&USwordAttackAbility::OnInputPressed);
	WaitInputPressedEvent->ReadyForActivation();
}

void USwordAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"Start SwordAttackAbility");
	
	AnimInstance = ActorInfo->GetAnimInstance();

	ShordTraceTask = UAbilityTask_SwordTrace::ShordTrace(this,12.f);
	if (IsValid(ShordTraceTask))
	{
		ShordTraceTask->Activate();
	}
	
	SetupInput();
	
	SetupComboEvents();
	
	ManageComboLogic();
}

void USwordAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"End SwordAttackAbility");
	if (WaitStartComboEvent)
	{
		WaitStartComboEvent->EndTask();
		WaitStartComboEvent = nullptr;
	}

	if (WaitEndComboEvent)
	{
		WaitEndComboEvent->EndTask();
		WaitEndComboEvent = nullptr;
	}
	
	if (WaitInputPressedEvent)
	{
		WaitInputPressedEvent->EndTask();
		WaitInputPressedEvent = nullptr;
	}
	
	Super::EndAbility(Handle,ActorInfo,ActivationInfo,bReplicateEndAbility, bWasCancelled);
}

void USwordAttackAbility::SetupComboEvents()
{
	WaitStartComboEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		RPGGameplayTags::Combat::ComboStates::Start
	);
	
	WaitStartComboEvent->EventReceived.AddDynamic(this, &USwordAttackAbility::PlayCombo);
	WaitStartComboEvent->ReadyForActivation();

	WaitEndComboEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		RPGGameplayTags::Combat::ComboStates::End
	);
	WaitEndComboEvent->EventReceived.AddDynamic(this, &USwordAttackAbility::StopCombo);
	WaitEndComboEvent->ReadyForActivation();
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
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple,FString::Printf(TEXT("Attack Combo: %d"),AttackIndex));
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

void USwordAttackAbility::OnInputPressed(float TimeWait)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Green,"Input Pressed");
	ManageComboLogic();
	SetupInput();
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
			case 0: AnimInstance->Montage_Play(Attack1); AttackIndex = 1; GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple,"Attack 1");
				break;
			case 1: AnimInstance->Montage_Play(Attack2); AttackIndex = 2; GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple,"Attack 2");
				break;
			case 2: AnimInstance->Montage_Play(Attack3); AttackIndex = 3; GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple,"Attack 3");
				break;
			case 3: AnimInstance->Montage_Play(Attack4); AttackIndex = 4; GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple,"Attack 4");
				break;
			default: break;
		}
	},0.1f,false);
}

