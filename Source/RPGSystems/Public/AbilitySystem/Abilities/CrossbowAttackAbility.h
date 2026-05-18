// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGDamageAbility.h"
#include "CrossbowAttackAbility.generated.h"

class ARPGSystemsCharacter;
class URPGAnimInstance;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_WaitInputPress;
class UAbilityTask_WaitInputRelease;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UCrossbowAttackAbility : public URPGDamageAbility
{
	GENERATED_BODY()
public:
protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
	UCrossbowAttackAbility();

	void SetupInputTask();
	void SetupMontageEvents();

	UFUNCTION()
	void ReadyToShoot(FGameplayEventData Payload);
	
	// UFUNCTION()
	// void OnInputPressed(float TimeWaited);

	UFUNCTION()
	void OnInputReleased(float TimeWaited);

	void SheathCrossbow();
	
	bool bIsReadyToShoot = false;

	FTimerHandle WaitUntilSheathTimer;
	
	UPROPERTY()
	TObjectPtr<URPGAnimInstance> AnimInstance;

	UPROPERTY()
	TObjectPtr<ARPGSystemsCharacter> OwnerCharacter;

	// UPROPERTY()
	// TObjectPtr<UAbilityTask_WaitInputPress> WaitInputPressedEvent;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputRelease> WaitInputReleasedEvent;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> WaitAimReady;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> AimCrossbowMontage;
};


