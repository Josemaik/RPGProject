// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "Data/RarityDataAsset.h"
#include "ItemTypes.generated.h"

class UEquipmentDefinition;
class UGameplayEffect;


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

	//Primary key
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemTag = FGameplayTag();
	
	//Display
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Display")
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Display")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Display")
	TSoftObjectPtr<UStaticMesh> ItemMesh;	

	//Clasification tags
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Classification")
	FGameplayTag CategoryTag;       // Item.Category.Equipment / Item.Category.Consumable

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Classification")
	FGameplayTag SubcategoryTag;    // Item.Subcategory.Weapon / Item.Subcategory.Armor

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Classification")
	FGameplayTag SlotTag;           // UI Inventory slots Item.Slot.SilverSword / Item.Slot.Consumables

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Classification")
	FGameplayTag TypeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Classification")
	FGameplayTag RarityTag;         // Item.Rarity.Common / Item.Rarity.Rare

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="CategoryTag == Item.Equipment", EditConditionHides))
	// EEquipmentType EquipmentType;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="CategoryTag == Item.Consumable", EditConditionHides))
	// EConsumableType ConsumableType;

	// chane to enum or gameplay tag
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
