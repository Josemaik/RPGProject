// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RPGGameplayAbility.h"
#include "TargetLockAbility.generated.h"

class ARPGSystemsCharacter;
class UAbilityTask_WaitInputPress;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UTargetLockAbility : public URPGGameplayAbility
{
	GENERATED_BODY()

	UTargetLockAbility();
	void CreateNewInputPressedEvent();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnInputPressed(float TimeWaited);

	void CalculateTargetLockedActor();

	UPROPERTY()
	TObjectPtr<ARPGSystemsCharacter> OwnedCharacter;

	UPROPERTY()
	TObjectPtr<AActor> LockedActorFinded;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputPress> WaitInputPressedTask;

	UPROPERTY(EditDefaultsOnly)
	float TraceRadius = 125.f;
	
	bool bIsLocked;
};
