// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/InventoryWidgetController.h"

#include "Interfaces/InventoryInterface.h"
#include "InventorySection/InventoryComponent.h"

void UInventoryWidgetController::SetOwningActor(AActor* InOwner)
{
	OwningActor = InOwner;
}

void UInventoryWidgetController::BindCallbacksToDependencies()
{
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(OwningActor);

	if (IsValid(OwningInventory))
	{
		OwningInventory->InventoryList.DirtyItemDelegate.AddLambda(
			[this](const FRPGInventoryEntry& DirtyItem)
			{
				InventoryEntryDelegate.Broadcast(DirtyItem);
			});

		OwningInventory->InventoryList.InventoryItemRemovedDelegate.AddLambda(
			[this](const FRPGInventoryEntry& ItemRemoved)
			{
				OnInventoryItemRemoved.Broadcast(ItemRemoved);
			});
		OwningInventory->OnWeightChanged.AddLambda(
			[this](float Weight)
			{
				OnInventoryWeightChanged.Broadcast(Weight);
			});
	}
}

void UInventoryWidgetController::BroadCastInitialValues() const
{
	if (IsValid(OwningInventory))
	{
		float TotalWeight = 0;
		for (const FRPGInventoryEntry& Entry : OwningInventory->GetInventoryEntries())
		{
			InventoryEntryDelegate.Broadcast(Entry);
			TotalWeight += Entry.Weight;
		}
		OnInventoryWeightChanged.Broadcast(TotalWeight);
	}
}
