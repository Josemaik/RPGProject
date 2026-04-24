// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InventorySection/ItemTypes.h"
#include "RarityDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FRarityData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FLinearColor Color;
	
	UPROPERTY(EditDefaultsOnly)
	FText RarityText;
};

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URarityDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TMap<EItemRarity, FRarityData> RarityColors;
};
