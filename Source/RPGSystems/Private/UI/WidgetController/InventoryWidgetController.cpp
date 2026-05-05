// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/InventoryWidgetController.h"

#include "Equipment/EquipmentManagerComponent.h"
#include "Interfaces/InventoryInterface.h"
#include "InventorySection/InventoryComponent.h"

void UInventoryWidgetController::SetOwningActor(AActor* InOwner)
{
	OwningActor = InOwner;
}

void UInventoryWidgetController::BindCallbacksToDependencies()
{
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(OwningActor);
	OwningEquipment = IInventoryInterface::Execute_GetEquipmentComponent(OwningActor);

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
			const FMasterItemDefinition& EntryDefinition = GetInventoryItemDefinition(Entry.ItemTag);
			InventoryEntryDelegate.Broadcast(Entry);
			TotalWeight += EntryDefinition.Weight;
		}
		OnInventoryWeightChanged.Broadcast(TotalWeight);
	}
}

void UInventoryWidgetController::EquipItem(const FRPGInventoryEntry& Entry) const
{
	if (!IsValid(OwningInventory)) return;
	
	OwningInventory->UseItem(Entry,1);
}

void UInventoryWidgetController::AddEquippedItem(FGameplayTag ItemTag,uint64 ExistingID) const
{
	if (!IsValid(OwningInventory)) return;
	
	OwningInventory->AddItem(ItemTag,ExistingID);
}

void UInventoryWidgetController::UnEquipItem(const FGameplayTag ItemTag) const
{
	if (!IsValid(OwningEquipment)) return;
	OwningEquipment->UnEquipItemByTag(ItemTag,EUnequipReason::Drop);
}

void UInventoryWidgetController::DropItemToWorld(const FRPGInventoryEntry& Entry) const
{
	if (!IsValid(OwningInventory)) return;

	OwningInventory->DropItem(Entry,Entry.Quantity);
}

void UInventoryWidgetController::RequestSortItems(bool Quick) const
{
	OnSortItemsRequested.Broadcast(Quick);
}

float UInventoryWidgetController::GetMaxInventoryWeight() const
{
	if (!IsValid(OwningInventory)) return 0;

	return OwningInventory->GetMaxInventoryWeight();
}

const FMasterItemDefinition UInventoryWidgetController::GetInventoryItemDefinition(FGameplayTag ItemTag) const
{
	return OwningInventory->GetItemDefinitionByTag(ItemTag);
}
