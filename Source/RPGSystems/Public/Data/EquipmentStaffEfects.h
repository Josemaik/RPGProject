// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "EquipmentStaffEfects.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UEquipmentStaffEfects : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, TObjectPtr<UDataTable>> MasterStatMap; 
};
