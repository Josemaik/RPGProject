// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventorySection/ItemTypes.h"
#include "ItemSlotDroppedDragDrop.generated.h"

class UEquipmentSlot;
struct FMasterItemDefinition;
enum ESlotSizeCategories : uint8;
enum class EItemRarity : uint8;
class UItemSlotWidget;
struct FRPGInventoryEntry;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemSlotDroppedDragDrop : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UItemSlotWidget* ItemSlot_Payload;
	
	UPROPERTY()
	UEquipmentSlot* SourceEquipmentSlot = nullptr;

	UPROPERTY()
	UItemSlotWidget* LastEnterSlotWidget;
	
	FRPGInventoryEntry* ItemEntry;

	FMasterItemDefinition ItemDefinition;

	// UPROPERTY()
	// UTexture2D* IconTexture;
	
	ESlotSizeCategories SlotSize;

	//EItemRarity Rarity;
};
