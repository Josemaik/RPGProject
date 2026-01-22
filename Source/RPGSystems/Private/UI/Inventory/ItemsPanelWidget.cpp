// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemsPanelWidget.h"

#include "IDetailTreeNode.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "UI/Inventory/ItemSlotWidget.h"

void UItemsPanelWidget::AddItem(UItemSlotWidget* Item)
{
	if (!IsValid(Item)) return;

	//int32& Index = CategoriesIndexMap.FindOrAdd(CurrentCategoryTag,0);
	
	TArray<UItemSlotWidget*>& ItemsArray = CategoryItemsMap.FindOrAdd(CurrentCategoryTag);

	const int32 Index = ItemsArray.Num();
	ItemsArray.Add(Item);
	
	const int32 Row = Index / MaxColumns;
	const int32 Column = Index % MaxColumns;
		
	UUniformGridSlot* GridSlot = ItemsPanel->AddChildToUniformGrid(Item);
		
	if (IsValid(GridSlot))
	{
		GridSlot->SetRow(Row);
		GridSlot->SetColumn(Column);
	}
		
	Item->SetGridSlot(GridSlot);
}

void UItemsPanelWidget::RemoveItem(const int64 ItemID)
{
	TArray<UItemSlotWidget*>& ItemsArray = CategoryItemsMap.FindOrAdd(CurrentCategoryTag);

	//Find Removed Slot
	int32 RemovedIndex = INDEX_NONE;
	UItemSlotWidget* RemovedSlot = nullptr;
	for (UItemSlotWidget* Item : ItemsArray)
	{
		if (IsValid(Item))
		{
			if (Item->ItemEntry.ItemID == ItemID)
			{
				RemovedIndex = ItemsArray.IndexOfByKey(Item);
				RemovedSlot = Item;
				break;
			}
		}
	}
	
	//Delete Slot from array and panel
	if (RemovedIndex != INDEX_NONE)
	{
		ItemsArray.RemoveAt(RemovedIndex);
		RemovedSlot->RemoveFromParent();
	}

	//compact the rest of items
	for (int32 i = RemovedIndex; i < ItemsArray.Num(); i++)
	{
		const int32 Row = i / MaxColumns;
		const int32 Column = i % MaxColumns;

		UItemSlotWidget* CurrentSlot = ItemsArray[i];
		if (!IsValid(CurrentSlot)) continue;
		
		if (UUniformGridSlot* GridSlot = CurrentSlot->GetGridSlot())
		{
			GridSlot->SetRow(Row);
			GridSlot->SetColumn(Column);
		}
	}
}

UItemSlotWidget* UItemsPanelWidget::ContainsItem(const int64 ItemID)
{
	if (!CategoryItemsMap.Contains(CurrentCategoryTag)) return nullptr;
	
	TArray<UItemSlotWidget*>& ItemsArray = CategoryItemsMap[CurrentCategoryTag];
	
	for (UItemSlotWidget* Item : ItemsArray)
	{
		if (IsValid(Item))
		{
			if (Item->ItemEntry.ItemID == ItemID)
			{
				return Item;
			}
		}
	}
	return nullptr;
}

void UItemsPanelWidget::UpdateItem(const FRPGInventoryEntry& Entry)
{
	if (UItemSlotWidget* UpdatedItem = ContainsItem(Entry.ItemID))
	{
		UpdatedItem->SetQuantityText(Entry.Quantity);
	}
}

void UItemsPanelWidget::ClearPanel()
{
	if (!IsValid(ItemsPanel)) return;
	
	ItemsPanel->ClearChildren();	
}

void UItemsPanelWidget::ResetCategory(FGameplayTag InCurrentCategoryTag)
{
	//Reset last category
	if (!CategoryItemsMap.Contains(CurrentCategoryTag)) return;
	CategoryItemsMap[CurrentCategoryTag].Empty();
	//set new category
	CurrentCategoryTag = InCurrentCategoryTag;
}

bool UItemsPanelWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                     UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("Drop on Items Panel")));
	return true;
}
