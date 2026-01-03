// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGDamageAbility.h"
#include "RPGGameplayAbility.h"
#include "SwordAttackAbility.generated.h"

class UAbilityTask_KickTrace;
class UAbilityTask_SwordTrace;
class UAbilityTask_WaitInputPress;
class UAbilityTask_WaitGameplayEvent;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API USwordAttackAbility : public URPGDamageAbility
{
	GENERATED_BODY()
public:
	USwordAttackAbility();
protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
	/*Combo functions*/
	void SetupInputTask();
	void SetupEventsTasks();
	void ManageComboLogic();
	void ChooseAttack();

	/*Gameplay Events Callbacks*/
	UFUNCTION()
	void PlayCombo(FGameplayEventData Payload);

	UFUNCTION(BlueprintCallable,meta=(allowPrivateAccess=true))
	void StopCombo(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnHitEventStart(FGameplayEventData Payload);

	UFUNCTION()
	void OnHitEventEnd(FGameplayEventData Payload);

	UFUNCTION()
	void OnKickHitEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnInputPressed(float TimeWait);

	UFUNCTION()
	void OnHitActor(AActor* HitActor,const FVector& HitLocation);
	
	/*Animation Data*/
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> Attack1;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> Attack2;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> Attack3;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> Attack4;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|CameraShake")
	TSubclassOf<UCameraShakeBase> SwordHitCameraShakeClass;
	
	//Gameplay Events
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitStartComboEvent;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitEndComboEvent;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitStartHitTrace;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitEndHitTrace;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitKickHit;
	//Ability Task - input and hit check
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputPress> WaitInputPressedEvent;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_SwordTrace> ShordTraceTask;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_KickTrace> KickHitTrace;

	/*Gameplay Effects*/
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Effects")
	TSubclassOf<UGameplayEffect> OnHitEffect;
	
	int32 AttackIndex;
	bool CanAttack = true;
	
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess=true))
	bool IsAttacking = false;
	
	bool saveAttack = false;
};



