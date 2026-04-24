// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "ItemTypes.generated.h"

class UEquipmentDefinition;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common  = 0 UMETA(DisplayName="Common"),
	Master  = 1 UMETA(DisplayName="Master"),
	Magic   = 2 UMETA(DisplayName="Magic"),
	Relic   = 3 UMETA(DisplayName="Relic"),
	Witcher = 4 UMETA(DisplayName="Witcher")
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Shield = 0,
	Axe = 1,
	Mace = 2,
	Crossbow  = 3,
	Sword = 4,
};

UENUM(BlueprintType)
enum class EConsumableType : uint8
{
	Food    = 0,
	Potion  = 1,
	Bomb    = 2,
	Oil     = 3
};

USTRUCT(BlueprintType)
struct FEquipmentItemProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UEquipmentDefinition> EquipmentClass = nullptr;
};

USTRUCT(BlueprintType)
struct FConsumableProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ItemEffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ItemEffectLevel = 1.f;
};

USTRUCT(BlueprintType)
struct FMasterItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EItemRarity Rarity = EItemRarity::Common;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="CategoryTag == Item.Equipment", EditConditionHides))
	EEquipmentType EquipmentType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="CategoryTag == Item.Consumable", EditConditionHides))
	EConsumableType ConsumableType;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FText ItemName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemDescription = FText();
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> ItemMesh = nullptr;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 SlotsSize = 0;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float Weight = 0.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float Price = 0.f;	

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FConsumableProps ConsumableProps = FConsumableProps();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FEquipmentItemProps EquipmentItemProps = FEquipmentItemProps();
};

USTRUCT(BlueprintType)
struct FPossibleLootParams: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ResultingItemtag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly)
	int32 MinNumItems = 1;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxNumItems = 1;

	UPROPERTY(EditDefaultsOnly)
	float ProbabilityToSelect = 0.f;
};
