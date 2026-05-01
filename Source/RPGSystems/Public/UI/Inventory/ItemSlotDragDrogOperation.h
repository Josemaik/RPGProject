// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventorySection/ItemTypes.h"
#include "ItemSlotDragDrogOperation.generated.h"

class UItemDragVisualWidget;
class UEquipmentSlot;
struct FMasterItemDefinition;
enum class ESlotSizeCategories : uint8;
enum class EItemRarity : uint8;
class UItemSlotWidget;
class UBaseInventorySlot;
struct FRPGInventoryEntry;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemSlotDragDrogOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	//Context
	UPROPERTY()
	UItemSlotWidget* ItemSlot_Payload;
	
	UPROPERTY()
	UEquipmentSlot* SourceEquipmentSlot = nullptr;

	UPROPERTY()
	UBaseInventorySlot* LastEnterSlotWidget; //only keep this

	//Visual
	UPROPERTY()
	UItemDragVisualWidget* ItemDraggedIconWidget;

	//Data
	FRPGInventoryEntry* ItemEntry;
	FMasterItemDefinition ItemDefinition;
	ESlotSizeCategories SlotSize;
	
};
