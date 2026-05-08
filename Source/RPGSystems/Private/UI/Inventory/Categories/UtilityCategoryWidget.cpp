// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Categories/UtilityCategoryWidget.h"
#include "Components/HorizontalBoxSlot.h"
#include "UI/Inventory/GridItemsPanelWidget.h"

void UUtilityCategoryWidget::ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
                                                   const FMasterItemDefinition& Definition)
{
	Super::ReceiveInventoryEntry(Entry, Definition);
}

void UUtilityCategoryWidget::RemoveEntry(const FRPGInventoryEntry& Entry)
{
	Super::RemoveEntry(Entry);
}

const FRPGInventoryEntry* UUtilityCategoryWidget::GetSelectedItem()
{
	return Super::GetSelectedItem();
}

void UUtilityCategoryWidget::NativeConstruct()
{
	if (!IsValid(QuestItemsPanel) || !IsValid(OthersPanel)) return;
	
	SubCategoryPanels.Add(QuestItemsPanel);
	SubCategoryPanels.Add(OthersPanel);

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


