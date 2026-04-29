// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Categories/EquipmentCategoryWidget.h"

#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemsPanelWidget.h"

void UEquipmentCategoryWidget::ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
                                                     const FMasterItemDefinition& Definition)
{
	if (Entry.ItemTag.MatchesTag(WeaponsTag))
	{
		WeaponsPanel->AddItemSlot(Entry, Definition);
	}
	else if (Entry.ItemTag.MatchesTag(ArmorTag))
	{
		ArmorPanel->AddItemSlot(Entry, Definition);
	}
}

void UEquipmentCategoryWidget::RemoveEntry(int64 ItemID)
{
	WeaponsPanel->RemoveItem(ItemID);
	ArmorPanel->RemoveItem(ItemID);
}

const FRPGInventoryEntry& UEquipmentCategoryWidget::GetSelectedItem()
{
	return WeaponsPanel->GetSelectedItem();
	//ArmorPanel->GetSelectedItem();
}
