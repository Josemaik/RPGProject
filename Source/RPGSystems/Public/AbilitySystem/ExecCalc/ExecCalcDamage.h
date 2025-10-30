// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalcDamage.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UExecCalcDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UExecCalcDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
