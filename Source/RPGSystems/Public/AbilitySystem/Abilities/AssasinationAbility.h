// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGDamageAbility.h"
#include "AssasinationAbility.generated.h"

class URPGMotionWarpingComponent;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UAssasinationAbility : public URPGDamageAbility
{
	GENERATED_BODY()
public:
	UAssasinationAbility();
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
	
	void PlayAssasinationAnimation(FVector& RefLocation,FRotator& AssasinRotation);
	
	UFUNCTION()
	void OnAssasinationMontageCompleted();

	UFUNCTION()
	void OnAssasinationMontageFailed();
	
	UPROPERTY()
	TWeakObjectPtr<UAnimInstance> AnimInstanceRef;

	UPROPERTY()
	TObjectPtr<AActor> ActorAssasinated;

	UPROPERTY()
	TWeakObjectPtr<URPGMotionWarpingComponent> MotionWarpingCompRef;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Animation")
	UAnimMontage* AssasinationAnimMontage;
};

