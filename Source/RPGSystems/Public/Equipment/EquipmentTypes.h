// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "EquipmentTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FEquipmentGrantedHandles
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilities = TArray<FGameplayAbilitySpecHandle>();

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> ActiveEffects = TArray<FActiveGameplayEffectHandle>();

	void AddAbilityHandle(FGameplayAbilitySpecHandle AbilitySpecHandle)
	{
		GrantedAbilities.Add(AbilitySpecHandle);
	}

	void AddEffectHandle(FActiveGameplayEffectHandle EffectHandle)
	{
		ActiveEffects.Add(EffectHandle);
	}
};

USTRUCT(BlueprintType)
struct FEquipmentStatEffectGroup : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag StatEffectTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UGameplayEffect> EffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinStatLevel = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxStatLevel = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bFractionalStat = false;

	UPROPERTY(BlueprintReadOnly)
	float CurrentValue = 0.f;
};
