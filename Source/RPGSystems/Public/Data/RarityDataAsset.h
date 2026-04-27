// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RarityDataAsset.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common  = 0 UMETA(DisplayName="Common"),
	Master  = 1 UMETA(DisplayName="Master"),
	Magic   = 2 UMETA(DisplayName="Magic"),
	Relic   = 3 UMETA(DisplayName="Relic"),
	Witcher = 4 UMETA(DisplayName="Witcher")
};

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
