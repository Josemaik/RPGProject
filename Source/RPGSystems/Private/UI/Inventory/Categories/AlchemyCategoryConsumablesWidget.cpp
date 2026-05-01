// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Categories/AlchemyCategoryConsumablesWidget.h"

#include "UI/Inventory/ItemsPanelWidget.h"

void UAlchemyCategoryConsumablesWidget::ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
															const FMasterItemDefinition& Definition)
{
	Super::ReceiveInventoryEntry(Entry, Definition);
}

void UAlchemyCategoryConsumablesWidget::RemoveEntry(int64 ItemID)
{
	Super::RemoveEntry(ItemID);
}

void UAlchemyCategoryConsumablesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (!IsValid(OilsPanel) || !IsValid(PotionsPanel) || !IsValid(BombsPanel)) return;

	SubCategoryPanels.Add(OilsPanel);
	SubCategoryPanels.Add(PotionsPanel);
	SubCategoryPanels.Add(BombsPanel);

	for (auto& Panel : SubCategoryPanels)
	{
		Panel->AddEmptySlots();
	}
	
	OilsPanel->OnSelectItemDelegate.BindLambda([this](int32 SelectedEntryIndex)
	{
		LastSelectedSubCategory = OilsPanel->GetSubCategoryTag();
	});
	PotionsPanel->OnSelectItemDelegate.BindLambda([this](int32 SelectedEntryIndex)
	{
		LastSelectedSubCategory = PotionsPanel->GetSubCategoryTag();
	});
	BombsPanel->OnSelectItemDelegate.BindLambda([this](int32 SelectedEntryIndex)
	{
		LastSelectedSubCategory = BombsPanel->GetSubCategoryTag();
	});
	
	

	// for (auto& Panel : SubCategoryPanels)
	// {
	// 	Panel->AddEmptySlots();
	// }
}
