// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGGameplayAbility.h"
#include "SwordAttackAbility.generated.h"

class UAbilityTask_WaitInputPress;
class UAbilityTask_WaitGameplayEvent;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API USwordAttackAbility : public URPGGameplayAbility
{
	GENERATED_BODY()
public:
	USwordAttackAbility();
protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	void SetupInput();
	void SetupComboEvents();
	void PlayFirstAttack();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:

	UFUNCTION()
	void PlayCombo(FGameplayEventData Payload);

	UFUNCTION()
	void StopCombo(FGameplayEventData Payload);

	UFUNCTION()
	void OnInputPressed(float TimeWait);
	
	void ChooseAttack();

	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitStartComboEvent;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitEndComboEvent;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputPress> WaitInputPressedEvent;
	
	int32 AttackIndex;
	bool CanAttack = true;
	bool IsAttacking = false;
	bool saveAttack = false;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> Attack1;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> Attack2;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> Attack3;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> Attack4;
	
};



