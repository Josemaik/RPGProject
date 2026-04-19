// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "RPGInputConfig.generated.h"

class UInputAction;

USTRUCT()
struct FRPGInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly)	//Abilities
	TArray<FRPGInputAction> RPGAbilityInputActions;

	UPROPERTY(EditDefaultsOnly) //Inventory
	TArray<FRPGInputAction> RPGInventoryInputActions;
	
	UPROPERTY(EditDefaultsOnly) //Gameplay Fast Access
	TArray<FRPGInputAction> RPGGameplayInputActions;
	
};
