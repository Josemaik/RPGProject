// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemSlotDroppedDragDrop.generated.h"

enum ESlotSizeCategories : uint8;
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
	UItemSlotWidget* LastEnterSlotWidget;

	FRPGInventoryEntry* ItemEntry;

	UPROPERTY()
	UTexture2D* IconTexture;
	
	ESlotSizeCategories SlotSize;
};
