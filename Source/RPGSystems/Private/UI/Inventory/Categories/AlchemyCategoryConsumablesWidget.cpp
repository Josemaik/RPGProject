// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Categories/AlchemyCategoryConsumablesWidget.h"

#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemsPanelWidget.h"

void UAlchemyCategoryConsumablesWidget::ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
                                                      const FMasterItemDefinition& Definition)
{
	if (Entry.ItemTag.MatchesTag(PotionsTag))
	{
		PotionsPanel->AddItemSlot(Entry, Definition);
		return;
	}
	
	if (Entry.ItemTag.MatchesTag(OilsTag))
	{
		OilsPanel->AddItemSlot(Entry, Definition);
		return;
	}

	if (Entry.ItemTag.MatchesTag(BombsTag))
	{
		BombsPanel->AddItemSlot(Entry, Definition);
	}
}

void UAlchemyCategoryConsumablesWidget::RemoveEntry(int64 ItemID)
{
	PotionsPanel->RemoveItem(ItemID);
	OilsPanel->RemoveItem(ItemID);
	BombsPanel->RemoveItem(ItemID);
}
