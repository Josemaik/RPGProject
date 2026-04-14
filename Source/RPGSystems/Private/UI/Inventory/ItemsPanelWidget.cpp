// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemsPanelWidget.h"

#include <rapidjson/reader.h>

#include "GameDelegates.h"
#include "IDetailTreeNode.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "UI/Inventory/ItemSlotDroppedDragDrop.h"
#include "UI/Inventory/ItemSlotWidget.h"

namespace FGameplayTags::Static
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Category_Equipment, "Item.Equipment");
}

void UItemsPanelWidget::AddItemToGrid(UItemSlotWidget* Item,const int32 Index)
{
	if (!IsValid(Item))
	{
		return;
	}
	
 	const int32 Row = Index / MaxColumns;
	const int32 Column = Index % MaxColumns;

	if (!IsValid(ItemsPanel))
	{
		return;
	}
	
	UUniformGridSlot* GridSlot = ItemsPanel->AddChildToUniformGrid(Item);
		
	if (IsValid(GridSlot))
	{
		GridSlot->SetRow(Row);
		GridSlot->SetColumn(Column);
	}
		
	Item->SetGridSlot(GridSlot);
	Item->SetGridIndex(Index);
}

void UItemsPanelWidget::RemoveItem(const int64 ItemID)
{
	TArray<UItemSlotWidget*>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);

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
		//ItemsArray.RemoveAt(RemovedIndex);
		RemovedSlot->EmptySlot();
	}

	//compact the rest of items
	for (int32 i = RemovedIndex; i < ItemsArray.Num() - 1; i++)
	{
		//const int32 Row = i / MaxColumns;
		//const int32 Column = i % MaxColumns;

		UItemSlotWidget* CurrentSlot = ItemsArray[i];
		if (!IsValid(CurrentSlot)) continue;

		UItemSlotWidget* NexSlot = ItemsArray[i + 1];
		
		if (!IsValid(NexSlot) || NexSlot->IsEmpty()) break;

		CurrentSlot->Init(NexSlot->ItemEntry,NexSlot->GetIconTexture());
		NexSlot->EmptySlot();
		// if (UUniformGridSlot* GridSlot = CurrentSlot->GetGridSlot())
		// {
		// 	GridSlot->SetRow(Row);
		// 	GridSlot->SetColumn(Column);
		// }
	}
}

UItemSlotWidget* UItemsPanelWidget::ContainsItemSlot(const int64 ItemID)
{
	if (!CategoryItemsMap.Contains(CurrentCategoryTag)) return nullptr;
	
	TArray<UItemSlotWidget*>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	
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

void UItemsPanelWidget::UpdateItemSlot(const FRPGInventoryEntry& Entry)
{
	if (UItemSlotWidget* UpdatedItem = ContainsItemSlot(Entry.ItemID))
	{
		UpdatedItem->SetQuantityText(Entry.Quantity);
	}
}

FGameplayTag UItemsPanelWidget::GetItemCategory(FGameplayTag ItemTag)
{
	FGameplayTag Parent = ItemTag;
	FGameplayTag Category = FGameplayTag();

	while (Parent.IsValid() && Parent != FGameplayTag::RequestGameplayTag(TEXT("Item")))
	{
		Category = Parent;
		Parent = Parent.RequestDirectParent();
	}

	return Category;
}

UItemSlotWidget* UItemsPanelWidget::AddItemSlot(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& ItemDefinition,int32 LastAddedSlotIndex)
{
	if (CategoryItemsMap.IsEmpty()) return nullptr;
	
	TArray<UItemSlotWidget*>& ItemsArray = *CategoryItemsMap.Find(GetItemCategory(Entry.ItemTag));
	if (ItemsArray.IsEmpty()) return nullptr;
	
	int32 FreeIndex = INDEX_NONE;
	FSlateBrush Brush;
	Brush.SetResourceObject(ItemDefinition.Icon.Get());
	bool isEquipmentSlot = false;

	//Equipment
	if (Entry.ItemTag.MatchesTag(FGameplayTags::Static::Category_Equipment) && ItemDefinition.SlotsSize == 2)
	{
		if (LastAddedSlotIndex != INDEX_NONE)
		{
			//Lower Slot Equipment
			FreeIndex = LastAddedSlotIndex + MaxColumns;
			if (!ItemsArray.IsValidIndex(FreeIndex))
			{
				return nullptr;
			}
			
			FBox2d UVRegionLower(FVector2d(0.0, 0.5), FVector2d(1.0, 1.0));
			Brush.SetUVRegion(UVRegionLower);

			UItemSlotWidget* NewItemSlot = ItemsArray[FreeIndex];
			NewItemSlot->Init(Entry,ItemDefinition.Icon,Brush,ESlotSizeCategories::LowerSlotVertical);
			return NewItemSlot;
		}
		
		// Superior Slot Equipment
		FBox2d UVRegionSuperior(FVector2d(0.0, 0.0), FVector2d(1.0, 0.5));
		Brush.SetUVRegion(UVRegionSuperior);
		
		isEquipmentSlot = true;
	}
	
	for (int32 i = 0; i < ItemsArray.Num(); ++i)
	{
		if (ItemsArray[i] && ItemsArray[i]->IsEmpty())
		{
			FreeIndex = i;
			break;
		}
	}

	if (FreeIndex != INDEX_NONE)
	{
		UItemSlotWidget* NewItemSlot = ItemsArray[FreeIndex];
		if (isEquipmentSlot)
		{
			NewItemSlot->Init(Entry,ItemDefinition.Icon,Brush,ESlotSizeCategories::SuperiorSlotVertical);
		}
		else
		{
			NewItemSlot->Init(Entry,ItemDefinition.Icon,Brush,ESlotSizeCategories::UniqueSlot);
		}
		return NewItemSlot;
	}
	
	return nullptr;
}

void UItemsPanelWidget::ClearPanel()
{
	if (!IsValid(ItemsPanel)) return;
	
	ItemsPanel->ClearChildren();	
}

void UItemsPanelWidget::AddEmptySlots(FGameplayTag InCurrentCategoryTag)
{
	TArray<UItemSlotWidget*>& ItemsArray = CategoryItemsMap.FindOrAdd(InCurrentCategoryTag,TArray<UItemSlotWidget*>());
	
	int32 RemainingSlots = NUM_INITIAL_EMPTY_SLOTS;
	while (RemainingSlots > 0)
	{
		UItemSlotWidget* CurrentItemSlotWidget = Cast<UItemSlotWidget>(CreateWidget(GetOwningPlayer(),ItemSlotWidgetClass));
		if (!IsValid(CurrentItemSlotWidget)) return;
		
		const int32 Index = ItemsArray.Num();
		ItemsArray.Add(CurrentItemSlotWidget);

		if (InCurrentCategoryTag.MatchesTagExact((CurrentCategoryTag)))
		{
			AddItemToGrid(CurrentItemSlotWidget,Index);
		}
		
		CurrentItemSlotWidget->OnItemDroppedPanelDelegate.BindUObject(this,&UItemsPanelWidget::HandleItemDropped);
		CurrentItemSlotWidget->OnDragEnteredDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemEntered);
		
		--RemainingSlots;
	}
}

void UItemsPanelWidget::ResetCategory(FGameplayTag InCurrentCategoryTag)
{
	CurrentCategoryTag = InCurrentCategoryTag;
	
	if (!IsValid(ItemsPanel)) return;
	//clear panel
	ItemsPanel->ClearChildren();

	TArray<UItemSlotWidget*>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;
	
	for (int i = 0; i < ItemsArray.Num(); i++)
	{
		UItemSlotWidget* CurrentItem = ItemsArray[i];
		if (IsValid(CurrentItem))
		{
			AddItemToGrid(CurrentItem,i);
		}
	}

	
	//Reset last category
	//if (!CategoryItemsMap.Contains(CurrentCategoryTag)) return;
	//CategoryItemsMap[CurrentCategoryTag].Empty();
	//set new category
}

bool UItemsPanelWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                     UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("Drop on Items Panel")));
	//InOperation->Payload
	// UItemSlotDroppedDragDrop* DragDropOperation = Cast<UItemSlotDroppedDragDrop>(InOperation);
	// if (!IsValid(DragDropOperation)) return false;
	
	return true;
}

void UItemsPanelWidget::HandleItemDropped(UItemSlotWidget* DroppedSlot,UItemSlotWidget* NewSlot)
{
	if (!IsValid(DroppedSlot) || !IsValid(NewSlot)) return;

	TArray<UItemSlotWidget*>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;
	
	bool IsDroppedSlotSuperior = DroppedSlot->GetCurrentSlotSize() == ESlotSizeCategories::SuperiorSlotVertical;

	//Set New Slot - Superior
	UItemSlotWidget* SuperiorSlot = IsDroppedSlotSuperior ? DroppedSlot : ItemsArray[DroppedSlot->GetGridIndex() - MaxColumns];
	if (!IsValid(SuperiorSlot)) return;
	NewSlot->Init(SuperiorSlot->ItemEntry,SuperiorSlot->GetIconTexture(),SuperiorSlot->GetIconBrush(),SuperiorSlot->GetCurrentSlotSize());

	UItemSlotWidget* NewLowerSlot = ItemsArray[NewSlot->GetGridIndex() + MaxColumns];
	UItemSlotWidget* OldLowerSlot = IsDroppedSlotSuperior ? ItemsArray[DroppedSlot->GetGridIndex() + MaxColumns] : DroppedSlot;
	NewLowerSlot->Init(OldLowerSlot->ItemEntry,OldLowerSlot->GetIconTexture(),OldLowerSlot->GetIconBrush(),OldLowerSlot->GetCurrentSlotSize());
	
	//Clear Dropped Slot
	OldLowerSlot->EmptySlot();
	SuperiorSlot->EmptySlot();
}

void UItemsPanelWidget::HandleDraggedItemEntered(int32 NewIndex)
{
	TArray<UItemSlotWidget*>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;

	UItemSlotWidget* NewLowerSlot = ItemsArray[NewIndex + MaxColumns];
	NewLowerSlot->OutlineSlot(ESlotSizeCategories::LowerSlotVertical);
}
