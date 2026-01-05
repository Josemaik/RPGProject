// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipmentTypes.h"
#include "UObject/Object.h"
#include "EquipmentGenerator.generated.h"

class UEquipmentDefinition;
class UEquipmentStaffEfects;
struct FGameplayTagContainer;
struct FEquipmentEffectPackage;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UEquipmentGenerator : public UObject
{
	GENERATED_BODY()

public:
	static void RollForStats(FEquipmentEffectPackage& OutPackage,const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
		TObjectPtr<UEquipmentStaffEfects> WeakStats);
private:
	static void RollPrefixOrSuffix(EEquipmentStatsGroup StatGroup, UEquipmentStaffEfects* StatEffects,
	FEquipmentEffectPackage& EffectPackage, FGameplayTagContainer& PossibleStatContainer);
};
