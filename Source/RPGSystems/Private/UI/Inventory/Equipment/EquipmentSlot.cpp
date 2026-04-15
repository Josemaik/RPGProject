// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Equipment/EquipmentSlot.h"

#include "Components/Image.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemSlotDroppedDragDrop.h"

bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                  UDragDropOperation* InOperation)
{
	UItemSlotDroppedDragDrop* DragOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!DragOp) return false;
	
	if (DragOp->ItemEntry->ItemTag.MatchesTag(EquipmentTag))
	{
		if (!bIsEmpty)
		{
			//Lógica para sustituir por el que se ha dragueado
			
		}

		bIsEmpty = false;
		
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Equipment dropped in Slot")));

		if (!IsValid(EquipmentSlotImage)) return false;
		EquipmentSlotImage->SetBrushFromTexture(DragOp->IconTexture);
		
		OnEquipItem.ExecuteIfBound(*DragOp->ItemEntry);
		
		return true;
	}
	
	return false;
}
