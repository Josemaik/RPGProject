// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "EquipmentTypes.generated.h"

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
