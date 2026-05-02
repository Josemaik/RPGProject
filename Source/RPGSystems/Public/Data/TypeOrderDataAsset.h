// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "TypeOrderDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FGameplayTagArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FGameplayTag> Tags;
};

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UTypeOrderDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// Weapons -> [Sword(0), Mace(1), Bow(2)...]
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, FGameplayTagArray> TypeOrderByCategory;
};
