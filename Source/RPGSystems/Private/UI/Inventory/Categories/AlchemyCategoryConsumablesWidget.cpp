// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Categories/AlchemyCategoryConsumablesWidget.h"

#include "UI/Inventory/GridItemsPanelWidget.h"

void UAlchemyCategoryConsumablesWidget::ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
                                                              const FMasterItemDefinition& Definition)
{
	Super::ReceiveInventoryEntry(Entry, Definition);
}

void UAlchemyCategoryConsumablesWidget::RemoveEntry(const FRPGInventoryEntry& Entry)
{
	Super::RemoveEntry(Entry);
}

void UAlchemyCategoryConsumablesWidget::NativeConstruct()
{
	if (!IsValid(OilsPanel) || !IsValid(PotionsPanel) || !IsValid(BombsPanel)) return;

	SubCategoryPanels.Add(OilsPanel);
	SubCategoryPanels.Add(PotionsPanel);
	SubCategoryPanels.Add(BombsPanel);

	for (auto& Panel : SubCategoryPanels)
	{
		Panel->AddEmptySlots();
	}
	Super::NativeConstruct();
}
