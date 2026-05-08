// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Categories/EquipmentCategoryWidget.h"

#include "Components/HorizontalBoxSlot.h"
#include "UI/Inventory/GridItemsPanelWidget.h"

void UEquipmentCategoryWidget::ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
                                                     const FMasterItemDefinition& Definition)
{
	Super::ReceiveInventoryEntry(Entry, Definition);
}

void UEquipmentCategoryWidget::RemoveEntry(const FRPGInventoryEntry& Entry)
{
	Super::RemoveEntry(Entry);
}

const FRPGInventoryEntry* UEquipmentCategoryWidget::GetSelectedItem()
{
	return Super::GetSelectedItem();
}

void UEquipmentCategoryWidget::NativeConstruct()
{
	if (!IsValid(WeaponsPanel) || !IsValid(ArmorPanel)) return;
	
	SubCategoryPanels.Add(WeaponsPanel);
	SubCategoryPanels.Add(ArmorPanel);

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


