// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Categories/FoodDrinkCategoryConsumableWidget.h"

#include "Components/HorizontalBoxSlot.h"
#include "UI/Inventory/ItemsPanelWidget.h"

void UFoodDrinkCategoryConsumableWidget::ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
													 const FMasterItemDefinition& Definition)
{
	Super::ReceiveInventoryEntry(Entry, Definition);
}

void UFoodDrinkCategoryConsumableWidget::RemoveEntry(const FRPGInventoryEntry& Entry)
{
	Super::RemoveEntry(Entry);
}

const FRPGInventoryEntry* UFoodDrinkCategoryConsumableWidget::GetSelectedItem()
{
	return Super::GetSelectedItem();
}

void UFoodDrinkCategoryConsumableWidget::NativeConstruct()
{
	if (!IsValid(FoodAndDrinkPanel)) return;
	
	SubCategoryPanels.Add(FoodAndDrinkPanel);

	for (auto& Panel : SubCategoryPanels)
	{
		Panel->AddEmptySlots();
		Panel->OnEquipmentDropped.BindLambda([this](FGameplayTag ItemTag,uint64 ExistingID)
		{
			OnEquipmentDropped.ExecuteIfBound(ItemTag,ExistingID);
		});
		if (UHorizontalBoxSlot* HSlot = Cast<UHorizontalBoxSlot>(Panel->Slot))
		{
			HSlot->SetHorizontalAlignment(HAlign_Left);
			HSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		}
	}

	Super::NativeConstruct();
}


