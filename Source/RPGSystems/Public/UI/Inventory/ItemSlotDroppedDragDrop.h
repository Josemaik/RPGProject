// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemSlotDroppedDragDrop.generated.h"

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
	FRPGInventoryEntry* ItemEntry;
	
	UPROPERTY()
	UItemSlotWidget* ItemSlot_Payload;

	UPROPERTY()
	TObjectPtr<UTexture2D> IconTexture;
};
