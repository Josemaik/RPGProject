// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RPGDamageAbility.h"
#include "CrossbowAttackAbility.generated.h"

class ACrossBowActor;
class UArrowComponent;
class AArrowActor;
class UAbilityTask_ArrowProjectileHits;
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

	UFUNCTION()
	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnInputPressed(float TimeWaited);

	UFUNCTION()
	void OnInputReleased(float TimeWaited);

	void OnCancelWindowFinished();
	void StartCancelWindow();
	void Reload();
	void ShootArrow();
	void SpawnArrow();
	
	bool bInputPressedInCalcelWindow = false;
	bool bIsCancelTimeActive = false;

	FTimerHandle WaitUntilSheathTimer;
	FTimerHandle WaitAfterShootTimer;
	
	UPROPERTY()
	TObjectPtr<URPGAnimInstance> AnimInstance;

	UPROPERTY()
	TObjectPtr<ARPGSystemsCharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<ACrossBowActor> CrossBowActor;

	// UPROPERTY()
	// TObjectPtr<UAbilityTask_WaitInputPress> WaitInputPressedEvent;
	UPROPERTY()
	TObjectPtr<UAbilityTask_ArrowProjectileHits> ArrowProjectileTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputRelease> WaitInputReleasedEvent;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputPress> WaitInputPressedEvent;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> AimCrossbowMontage;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	TObjectPtr<UAnimMontage> ReloadCrossbowMontage;
};


