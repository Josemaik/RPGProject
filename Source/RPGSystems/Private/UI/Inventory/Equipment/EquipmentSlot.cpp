// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Equipment/EquipmentSlot.h"

#include "AbilitySystem/RPGGameplayTags.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemSlotDroppedDragDrop.h"

bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                  UDragDropOperation* InOperation)
{
	UItemSlotDroppedDragDrop* DragOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!DragOp) return false;

	if (DragOp->ItemEntry->ItemTag.MatchesTag(EquipmentTag))
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Equipment dropped in Slot")));
		OnEquipItem.ExecuteIfBound(*DragOp->ItemEntry);
		return true;
	}
	
	return false;
}
