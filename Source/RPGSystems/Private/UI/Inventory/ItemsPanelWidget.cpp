// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemsPanelWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Engine/AssetManager.h"
#include "UI/Inventory/InventoryWidget.h"
#include "UI/Inventory/ItemSlotDroppedDragDrop.h"
#include "UI/Inventory/ItemSlotWidget.h"
#include "UI/Inventory/SortPanelWidget.h"
#include "AbilitySystem/NativeTags/RPGInventoryTags.h"

struct FStreamableManager;


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
	TArray<FItemSlotData>* ItemsArrayPtr = CategoryItemsMap.Find(CurrentCategoryTag);
	if (!ItemsArrayPtr) return;
	TArray<FItemSlotData>& ItemsArray = *ItemsArrayPtr;
	for (int i = 0; i < ItemsArray.Num();i++)
	{
		FItemSlotData& ItemSlotData = ItemsArray[i];
		if (!ItemSlotData.bIsEmpty && ItemSlotData.Entry.ItemID == ItemID)
		{
			ItemSlotData.bIsEmpty = true;
			if (ItemSlotData.Size == SuperiorSlotVertical)
			{
				ItemsArray[i + MaxColumns].bIsEmpty = true;
			}
			if (ItemSlotData.Size == LowerSlotVertical)
			{
				ItemsArray[i - MaxColumns].bIsEmpty = true;
			}
			break;
		}
	}

	ResetCategory(CurrentCategoryTag);
}

int32 UItemsPanelWidget::FindItemIndex(const int64 ItemID,FGameplayTag ItemTag)
{
	TArray<FItemSlotData>* ItemsArrayPtr = CategoryItemsMap.Find(GetItemCategory(ItemTag));
	
	if (!ItemsArrayPtr) return INDEX_NONE;

	for (int32 i = 0; i < ItemsArrayPtr->Num(); i++)
	{
		if (!(*ItemsArrayPtr)[i].bIsEmpty &&
			(*ItemsArrayPtr)[i].Entry.ItemID == ItemID)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

void UItemsPanelWidget::UpdateItemSlot(const FRPGInventoryEntry& Entry)
{
	TArray<FItemSlotData>* ItemsArrayPtr = CategoryItemsMap.Find(GetItemCategory(Entry.ItemTag));
	if (!ItemsArrayPtr) return;

	for (FItemSlotData& ItemSlotData : *ItemsArrayPtr)
	{
		if (!ItemSlotData.bIsEmpty && ItemSlotData.Entry.ItemID == Entry.ItemID)
		{
			ItemSlotData.Entry = Entry;
			break;
		}
	}

	ResetCategory(CurrentCategoryTag);
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

void UItemsPanelWidget::AddItemSlot(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& ItemDefinition,bool bResetPanel)
{
	TArray<FItemSlotData>* ItemsArrayPtr = CategoryItemsMap.Find(GetItemCategory(Entry.ItemTag));
	if (!ItemsArrayPtr) return;

	TArray<FItemSlotData>& ItemsArray = *ItemsArrayPtr;

	int32 FreeIndex = INDEX_NONE;

	for (int32 i = 0; i < ItemsArray.Num(); i++)
	{
		bool EntrySizeHiguerThanOne = ItemDefinition.SlotsSize > 1;
		bool IsUniqueItem = !EntrySizeHiguerThanOne && ItemsArray[i].bIsEmpty;
		bool IsEquipmentItem = EntrySizeHiguerThanOne && ItemsArray[i].bIsEmpty && ItemsArray[i + MaxColumns].bIsEmpty;
		
		if (IsUniqueItem || IsEquipmentItem)
		{
			FreeIndex = i;
			break;
		}
	}

	if (FreeIndex == INDEX_NONE) return;

	ItemsArray[FreeIndex].Entry = Entry;
	ItemsArray[FreeIndex].Icon = ItemDefinition.Icon;
	ItemsArray[FreeIndex].bIsEmpty = false;
	ItemsArray[FreeIndex].Size = UniqueSlot;
	ItemsArray[FreeIndex].ItemDefinition = ItemDefinition;
	
	FSlateBrush Brush;
	Brush.SetResourceObject(ItemDefinition.Icon.Get());
	
	if (Entry.ItemTag.MatchesTag(RPGInventoryTags::ItemsCategory::Equipment) && ItemDefinition.SlotsSize == 2)
	{   //Slot Size == 2
		ItemsArray[FreeIndex].Size = SuperiorSlotVertical;
		ItemsArray[FreeIndex + MaxColumns].Entry = Entry;
		ItemsArray[FreeIndex + MaxColumns].Icon = ItemDefinition.Icon;
		ItemsArray[FreeIndex + MaxColumns].bIsEmpty = false;
		ItemsArray[FreeIndex + MaxColumns].Size = LowerSlotVertical;
		ItemsArray[FreeIndex + MaxColumns].ItemDefinition = ItemDefinition;
	}
	
	if (GetItemCategory(Entry.ItemTag) == CurrentCategoryTag && bResetPanel)
	{
		ResetCategory(CurrentCategoryTag);
	}
}

void UItemsPanelWidget::ClearPanel()
{
	if (!IsValid(ItemsPanel)) return;
	
	ItemsPanel->ClearChildren();	
}

void UItemsPanelWidget::AddEmptySlots(FGameplayTag InCurrentCategoryTag)
{
	TArray<FItemSlotData>& ItemsArray = CategoryItemsMap.FindOrAdd(InCurrentCategoryTag);

	while (ItemsArray.Num() < NUM_INITIAL_EMPTY_SLOTS)
	{
		FItemSlotData NewSlot;
		NewSlot.bIsEmpty = true;
		ItemsArray.Add(NewSlot);
	}

	// Si es la categoría actual → reconstruir UI
	if (InCurrentCategoryTag.MatchesTagExact(CurrentCategoryTag))
	{
		ResetCategory(CurrentCategoryTag);
	}
}

void UItemsPanelWidget::ProcessSlotWidget(const FItemSlotData& SlotData, UItemSlotWidget* NewWidget, FSlateBrush Brush)
{
	switch (SlotData.Size)
	{
	case LowerSlotVertical:
		{
			FBox2d UVRegionLower(FVector2d(0.0, 0.5), FVector2d(1.0, 1.0));
			Brush.SetUVRegion(UVRegionLower);

			NewWidget->Init(SlotData.Entry, SlotData.Icon, Brush, LowerSlotVertical);
			break;
		}
	case SuperiorSlotVertical:
		{
			FBox2d UVRegionSuperior(FVector2d(0.0, 0.0), FVector2d(1.0, 0.5));
			Brush.SetUVRegion(UVRegionSuperior);
							
			NewWidget->Init(SlotData.Entry, SlotData.Icon, Brush, SuperiorSlotVertical);
			break;
		}
	case UniqueSlot: { NewWidget->Init(SlotData.Entry, SlotData.Icon, Brush, UniqueSlot); break; }
	default: break;
	}
}

void UItemsPanelWidget::CreateSlotWidget(int32 Index,const FItemSlotData& SlotData)
{
	UItemSlotWidget* NewWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), ItemSlotWidgetClass);
	if (!IsValid(NewWidget)) return;

	if (!SlotData.bIsEmpty)
	{	//Item Slot
		FSlateBrush Brush;
		if (SlotData.Icon.IsNull())
		{
			return;
		}

		if (SlotData.Icon.IsValid())
		{
			Brush.SetResourceObject(SlotData.Icon.Get());
			ProcessSlotWidget(SlotData, NewWidget, Brush);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Icon texture Not Loaded")));
			FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

			Streamable.RequestAsyncLoad(SlotData.Icon.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([this, SlotData,NewWidget,Index]()
				{
					if (UTexture2D* LoadedTexture = SlotData.Icon.Get())
					{
						FSlateBrush Brush;
						Brush.SetResourceObject(LoadedTexture);
						
						ProcessSlotWidget(SlotData, NewWidget, Brush);

						//Bind Callbacks and Add to Grid Panel
						NewWidget->OnItemDroppedPanelDelegate.BindUObject(this,&UItemsPanelWidget::HandleItemDropped);
						NewWidget->OnDragEnteredDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemEntered);
						NewWidget->OnDragLeavedDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemLeaved);
						NewWidget->OnDragCancelledDelegate.BindUObject(this,&UItemsPanelWidget::HandleDragCancelled);
						AddItemToGrid(NewWidget, Index);
					}
				})
			);
			return;
		}
	}
	else
	{	//Empty Slot
		NewWidget->EmptySlot();
	}
	
	//Bind Callbacks and Add to Grid Panel
	NewWidget->OnItemDroppedPanelDelegate.BindUObject(this,&UItemsPanelWidget::HandleItemDropped);
	NewWidget->OnDragEnteredDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemEntered);
	NewWidget->OnDragLeavedDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemLeaved);
	NewWidget->OnDragCancelledDelegate.BindUObject(this,&UItemsPanelWidget::HandleDragCancelled);
	AddItemToGrid(NewWidget, Index);
}

void UItemsPanelWidget::ResetCategory(FGameplayTag InCurrentCategoryTag)
{
	CurrentCategoryTag = InCurrentCategoryTag;

	
	if (!IsValid(ItemsPanel)) return;
	//clear panel
	ItemsPanel->ClearChildren();

	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;

	for (int32 i = 0; i < ItemsArray.Num(); i++)
	{
		const FItemSlotData& SlotData = ItemsArray[i];

		CreateSlotWidget(i,SlotData);
	}
}

void UItemsPanelWidget::SortItemsQuicly()
{
	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;
	
	TArray<FLogicalItem> LogicalItems;

	for (int32 i = 0; i < ItemsArray.Num(); i++)
	{
		const FItemSlotData& ItemSlot = ItemsArray[i];

		if (ItemSlot.bIsEmpty) continue;

		// get item root
		if (ItemSlot.Size == LowerSlotVertical) continue;

		FLogicalItem Item;
		Item.Entry = ItemSlot.Entry;
		Item.ItemDefinition = ItemSlot.ItemDefinition;
		Item.Size = (ItemSlot.Size == SuperiorSlotVertical) ? 2 : 1;

		LogicalItems.Add(Item);
	}

	//Sort by size
	LogicalItems.Sort([](const FLogicalItem& A, const FLogicalItem& B)
	{
		return A.Size > B.Size;
	});

	//clean items array
	for (FItemSlotData& ItemSlot : ItemsArray)
	{
		ItemSlot = FItemSlotData();
		ItemSlot.bIsEmpty = true;
	}

	//Add sorted slots
	for (const FLogicalItem& Item : LogicalItems)
	{
		AddItemSlot(Item.Entry, Item.ItemDefinition,false);
	}

	ResetCategory(CurrentCategoryTag);
}

void UItemsPanelWidget::SortItemsBy(EItemSortType SortType)
{
	TArray<FItemSlotData>* ItemsArrayPtr = CategoryItemsMap.Find(CurrentCategoryTag);
	if (!ItemsArrayPtr || ItemsArrayPtr->IsEmpty()) return;

	TArray<FItemSlotData> SortedItems;

	for (const FItemSlotData& ItemSlot : *ItemsArrayPtr)
	{
		if (ItemSlot.bIsEmpty) continue;
		if (ItemSlot.Size == LowerSlotVertical) continue; //Skip lower and empties slots
		SortedItems.Add(ItemSlot);
	}
	
	switch (SortType)
	{
		case EItemSortType::Weight :
		{
				SortedItems.Sort([](const FItemSlotData& A, const FItemSlotData& B)
				{
					return A.ItemDefinition.Weight > B.ItemDefinition.Weight;
				});
				break;
		}
	case EItemSortType::Price :
			{
				SortedItems.Sort([](const FItemSlotData& A, const FItemSlotData& B)
				{
					return A.ItemDefinition.Price > B.ItemDefinition.Price;
				});
				break;
			}
	case EItemSortType::Rarity :
			{
				SortedItems.Sort([&](const FItemSlotData& A, const FItemSlotData& B)
				{
					if (A.ItemDefinition.Rarity != B.ItemDefinition.Rarity)
						return A.ItemDefinition.Rarity > B.ItemDefinition.Rarity;
					
					if (CurrentCategoryTag == RPGInventoryTags::ItemsCategory::Consumable)
					{
						return A.ItemDefinition.ConsumableType > B.ItemDefinition.ConsumableType;
					}
					return A.ItemDefinition.EquipmentType > B.ItemDefinition.EquipmentType;
				});
				break;
			}
		case EItemSortType::Type :
			{
				SortedItems.Sort([&](const FItemSlotData& A, const FItemSlotData& B)
					{
						if (CurrentCategoryTag == RPGInventoryTags::ItemsCategory::Consumable)
						{
							if (A.ItemDefinition.ConsumableType != B.ItemDefinition.ConsumableType)
								return A.ItemDefinition.ConsumableType > B.ItemDefinition.ConsumableType;
						}
						else
						{
							if (A.ItemDefinition.EquipmentType != B.ItemDefinition.EquipmentType)
								return A.ItemDefinition.EquipmentType > B.ItemDefinition.EquipmentType;
						}

						return A.ItemDefinition.Rarity > B.ItemDefinition.Rarity;
				});
				break;
			}
		default: break;
	}
	// Clean
	for (FItemSlotData& ItemSlot : *ItemsArrayPtr)
	{
		ItemSlot = FItemSlotData();
		ItemSlot.bIsEmpty = true;
	}

	// Re-add sorted
	for (const FItemSlotData& Item : SortedItems)
	{
		AddItemSlot(Item.Entry, Item.ItemDefinition,false);
	}

	ResetCategory(CurrentCategoryTag);
}

// -------------------------------------
//  Drag & Drop – Panel-side logic
// -------------------------------------

void UItemsPanelWidget::HandleItemDropped(int32 DroppedIndex,int32 NewIndex)
{
	if (TryDropInNewSlot(DroppedIndex,NewIndex))
	{
		ResetCategory(CurrentCategoryTag);
	}

	//call function that drops fail
}

bool UItemsPanelWidget::TryDropInNewSlot(int32 FromIndex,int32 ToIndex)
{
	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return false;
	if (!ItemsArray.IsValidIndex(FromIndex) || !ItemsArray.IsValidIndex(ToIndex)) return false;
	
	ESlotSizeCategories FromSize = ItemsArray[FromIndex].Size;
	FItemSlotData& ToSlot = ItemsArray[ToIndex];

	//From Index is unique slot
	if (FromSize == UniqueSlot)
	{
		if (ToSlot.bIsEmpty)
		{
			ToSlot = ItemsArray[FromIndex];
			ResetSlotData(ItemsArray[FromIndex]);
			return true;
		}
		if (ToSlot.Size == UniqueSlot && ToIndex != FromIndex)
		{
			ItemsArray.Swap(ToIndex,FromIndex);
			return true;
		}
		return false;
	}
	
	
	//From Index is part of a two size slot
	if (ToSlot.Size == UniqueSlot && !ToSlot.bIsEmpty) return false;

	const int32 FromSuperiorIndex = (FromSize == SuperiorSlotVertical) ? FromIndex : FromIndex - MaxColumns;
	const int32 FromLowerIndex    = FromSuperiorIndex + MaxColumns;

	if (!ItemsArray.IsValidIndex(FromLowerIndex)) return false;
	
	const int32 ToSuperiorIndex = (ToSlot.Size == LowerSlotVertical) ? ToIndex - MaxColumns : ToIndex;
	const int32 ToLowerIndex    = ToSuperiorIndex + MaxColumns;

	if (!ItemsArray.IsValidIndex(ToLowerIndex)) return false;

	FItemSlotData& ToSuperiorSlot = ItemsArray[ToSuperiorIndex];
	FItemSlotData& ToLowerSlot    = ItemsArray[ToLowerIndex];
	
	if (ToSuperiorSlot.bIsEmpty && ToLowerSlot.bIsEmpty)
	{
		ToSuperiorSlot = ItemsArray[FromSuperiorIndex];
		ToLowerSlot    = ItemsArray[FromLowerIndex];
		ResetSlotData(ItemsArray[FromSuperiorIndex]);
		ResetSlotData(ItemsArray[FromLowerIndex]);
		return true;
	}

	if (ToSuperiorSlot.Size == SuperiorSlotVertical || ToSuperiorSlot.Size == LowerSlotVertical)
	{
		if (ToSuperiorIndex == FromSuperiorIndex) return false; // same item
 
		// Swap both halves
		ItemsArray.Swap(FromSuperiorIndex, ToSuperiorIndex);
		ItemsArray.Swap(FromLowerIndex,    ToLowerIndex);
		return true;
	}
	
	return false;
}

void UItemsPanelWidget::SetSlotOutline(int32 NewIndex,bool enable,ESlotSizeCategories Size) const
{
	UItemSlotWidget* ItemSlotWidget = GetItemSlotbyIndex(NewIndex);
	if (!IsValid(ItemSlotWidget)) return;
	
	enable ? ItemSlotWidget->OutlineSlot(Size) :
	ItemSlotWidget->RemoveOutLineSlot(false);
}

void UItemsPanelWidget::HandleDraggedItemEntered(int32 DraggedIndex, int32 TargetIndex)
{
	if (!ItemsPanel) return;
	
	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;
	if (!ItemsArray.IsValidIndex(DraggedIndex) || !ItemsArray.IsValidIndex(TargetIndex)) return;

	const FItemSlotData& DraggedSlot = ItemsArray[DraggedIndex];
	const FItemSlotData& TargetSlot  = ItemsArray[TargetIndex];

	const int32 TargetSuperiorIndex = (TargetSlot.Size == LowerSlotVertical)? TargetIndex - MaxColumns : TargetIndex;
	const int32 TargetLowerIndex = TargetSuperiorIndex + MaxColumns;

	if (!ItemsArray.IsValidIndex(TargetLowerIndex)) return;

	const FItemSlotData& TargetSuperiorSlot = ItemsArray[TargetSuperiorIndex];
	const FItemSlotData& TargetLowerSlot    = ItemsArray[TargetLowerIndex];
	
	const int32 DraggedSuperiorIndex = (DraggedSlot.Size == SuperiorSlotVertical) ? DraggedIndex : DraggedIndex - MaxColumns;

	EDragOverResult Result = EDragOverResult::Invalid;

	if (TargetSlot.Size == UniqueSlot && !TargetSlot.bIsEmpty)
	{
		// Can't fit a 2-slot item on a unique slot
		Result = EDragOverResult::Invalid;
	}else if (TargetSuperiorSlot.bIsEmpty && TargetLowerSlot.bIsEmpty)
	{
		// Both target cells are free → drop
		Result = EDragOverResult::Drop;
	}else if (TargetSuperiorSlot.Size == SuperiorSlotVertical || TargetSuperiorSlot.Size == LowerSlotVertical)
	{
		// Target is another 2-slot item
		if (TargetSuperiorIndex == DraggedSuperiorIndex)
		{
			// Hovering over self – for the "inferior" half case described in spec:
			// if TargetIndex == DroppedIndex → allow (same item, just show drop)
			Result = EDragOverResult::Drop;
		}
		else
		{
			Result = EDragOverResult::Swap;
		}
	}

	UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(TargetSuperiorIndex);
	UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(TargetLowerIndex);
 
	if (IsValid(TopWidget))    TopWidget->EnableDragOverPreview(Result);
	if (IsValid(BottomWidget)) BottomWidget->EnableDragOverPreview(Result);
}

UItemSlotWidget* UItemsPanelWidget::GetItemSlotbyIndex(int32 Index) const
{
	if (UWidget* Child = ItemsPanel->GetChildAt(Index))
	{
		if (UItemSlotWidget* ItemSlotWidget = Cast<UItemSlotWidget>(Child))
		{
			return ItemSlotWidget;
		}
	}
	return nullptr;
}

void UItemsPanelWidget::HandleDraggedItemLeaved(int32 DraggedIndex,int32 TargetIndex)
{
	const TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;
	if (!ItemsArray.IsValidIndex(TargetIndex)) return;
 
	const FItemSlotData& TargetSlot = ItemsArray[TargetIndex];
 
	// Resolve the superior index so we always disable the right pair
	const int32 TargetSuperiorIndex = (TargetSlot.Size == LowerSlotVertical) ? TargetIndex - MaxColumns : TargetIndex;
	const int32 TargetLowerIndex    = TargetSuperiorIndex + MaxColumns;

	UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(TargetSuperiorIndex);
	UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(TargetLowerIndex);
 
	if (IsValid(TopWidget))    TopWidget->DisableDragOverPreview();
	if (IsValid(BottomWidget)) BottomWidget->DisableDragOverPreview();
}

void UItemsPanelWidget::HandleDragCancelled(int LastEnterIndex) const
{
	const TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;
	if (!ItemsArray.IsValidIndex(LastEnterIndex)) return;
 
	const FItemSlotData& LastEnterSlot = ItemsArray[LastEnterIndex];
 
	// Resolve the superior index so we always disable the right pair
	const int32 TargetSuperiorIndex = (LastEnterSlot.Size == LowerSlotVertical) ? LastEnterIndex - MaxColumns : LastEnterIndex + MaxColumns;

	UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(TargetSuperiorIndex);
	UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(LastEnterIndex);
 
	if (IsValid(TopWidget))    TopWidget->DisableDragOverPreview();
	if (IsValid(BottomWidget)) BottomWidget->DisableDragOverPreview();
}

void UItemsPanelWidget::ResetSlotData(FItemSlotData& ItemSlotData)
{
	ItemSlotData.Entry = FRPGInventoryEntry();
	ItemSlotData.Icon = nullptr;
	ItemSlotData.Size = UniqueSlot;
	ItemSlotData.bIsEmpty = true;
	ItemSlotData.ItemDefinition = FMasterItemDefinition();
	ItemSlotData.IconBrush = FSlateBrush();
}

