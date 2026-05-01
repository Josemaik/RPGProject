// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Categories/BaseCategoryWidget.h"
#include "UI/Inventory/ItemsPanelWidget.h"

void UBaseCategoryWidget::ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
	const FMasterItemDefinition& Definition)
{
	for (UItemsPanelWidget* Panel : SubCategoryPanels)
	{
		if (!IsValid(Panel)) return;
		if (Definition.SubcategoryTag.MatchesTagExact(Panel->GetSubCategoryTag()))
		{
			Panel->AddItemSlot(Entry, Definition);
		}
	}
}

void UBaseCategoryWidget::RemoveEntry(int64 ItemID)
{
	for (UItemsPanelWidget* Panel : SubCategoryPanels)
	{
		if (!IsValid(Panel)) return;
		Panel->RemoveItem(ItemID);
	}
}

const FRPGInventoryEntry* UBaseCategoryWidget::GetSelectedItem()
{
	for (UItemsPanelWidget* Panel : SubCategoryPanels)
	{
		if (LastSelectedSubCategory.MatchesTagExact(Panel->GetSubCategoryTag()))
		{
			return Panel->GetSelectedItem();
		}
	}
	return nullptr;
}
