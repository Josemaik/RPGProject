// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemsPanelWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Engine/AssetManager.h"
#include "UI/Inventory/InventoryWidget.h"
#include "UI/Inventory/ItemSlotDragDrogOperation.h"
#include "UI/Inventory/ItemSlotWidget.h"
#include "UI/Inventory/SortPanelWidget.h"
#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "UI/Inventory/ItemDragVisualWidget.h"
#include "UI/Inventory/ItemToolTip.h"
#include "UI/Inventory/Equipment/EquipmentSlot.h"

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
	ItemToolTipReference->SetVisibility(ESlateVisibility::Collapsed);
	bool ItemRemoved = false;
	TArray<FItemSlotData>* ItemsArrayPtr = CategoryItemsMap.Find(CurrentCategoryTag);
	if (!ItemsArrayPtr) return;
	TArray<FItemSlotData>& ItemsArray = *ItemsArrayPtr;
	for (int i = 0; i < ItemsArray.Num();i++)
	{
		FItemSlotData& ItemSlotData = ItemsArray[i];
		if (!ItemSlotData.bIsEmpty && ItemSlotData.Entry.ItemID == ItemID)
		{
			if (ItemSlotData.Size == SuperiorSlotVertical)
			{
				ResetSlotData(ItemsArray[i + MaxColumns]);
			}
			if (ItemSlotData.Size == LowerSlotVertical)
			{
				ResetSlotData(ItemsArray[i - MaxColumns]);
			}
			ResetSlotData(ItemSlotData);
			ItemRemoved = true;
			break;
		}
	}
	if (!ItemRemoved) return;
	ResetCategory(CurrentCategoryTag);
}

int32 UItemsPanelWidget::FindGridIndexByItemID(const int64 ItemID,FGameplayTag ItemTag)
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

	CurrentSelectedIndex = FreeIndex;
	
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

void UItemsPanelWidget::SelectSlotAtIndex(int32 Index)
{
	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (!ItemsArray.IsValidIndex(Index)) return;

	// Deselect old last selected
	DeselectCurrentSlot();

	CurrentSelectedIndex = Index;
	
	UItemSlotWidget* SlotWidget = GetItemSlotbyIndex(Index);
	if (IsValid(Slot)) SlotWidget->StartSelectedAnimation();

	const FItemSlotData& SlotData = ItemsArray[Index];
	if (SlotData.Size == SuperiorSlotVertical)
	{
		UItemSlotWidget* LowerWidget = GetItemSlotbyIndex(Index + MaxColumns);
		if (IsValid(LowerWidget)) LowerWidget->StartSelectedAnimation();
	}
	else if (SlotData.Size == LowerSlotVertical)
	{
		UItemSlotWidget* SuperiorWidget = GetItemSlotbyIndex(Index - MaxColumns);
		if (IsValid(SuperiorWidget)) SuperiorWidget->StartSelectedAnimation();
	}
}

void UItemsPanelWidget::DeselectCurrentSlot()
{
	if (CurrentSelectedIndex == INDEX_NONE) return;

	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (!ItemsArray.IsValidIndex(CurrentSelectedIndex)) return;

	UItemSlotWidget* SlotWidget = GetItemSlotbyIndex(CurrentSelectedIndex);
	if (IsValid(SlotWidget)) SlotWidget->StopSelectedAnimation();

	const FItemSlotData& SlotData = ItemsArray[CurrentSelectedIndex];
	if (SlotData.Size == SuperiorSlotVertical)
	{
		UItemSlotWidget* LowerWidget = GetItemSlotbyIndex(CurrentSelectedIndex + MaxColumns);
		if (IsValid(LowerWidget)) LowerWidget->StopSelectedAnimation();
	}
	else if (SlotData.Size == LowerSlotVertical)
	{
		UItemSlotWidget* SuperiorWidget = GetItemSlotbyIndex(CurrentSelectedIndex - MaxColumns);
		if (IsValid(SuperiorWidget)) SuperiorWidget->StopSelectedAnimation();
	}

	CurrentSelectedIndex = INDEX_NONE;
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

void UItemsPanelWidget::ResetSlotData(FItemSlotData& ItemSlotData)
{
	ItemSlotData.Entry = FRPGInventoryEntry();
	ItemSlotData.Icon = nullptr;
	ItemSlotData.Size = UniqueSlot;
	ItemSlotData.bIsEmpty = true;
	ItemSlotData.ItemDefinition = FMasterItemDefinition();
	ItemSlotData.IconBrush = FSlateBrush();
}

void UItemsPanelWidget::InitializeSlotWidget(const FItemSlotData& SlotData, UItemSlotWidget* NewWidget, FSlateBrush Brush)
{
	switch (SlotData.Size)
	{
	case LowerSlotVertical:
		{
			FBox2d UVRegionLower(FVector2d(0.0, 0.5), FVector2d(1.0, 1.0));
			Brush.SetUVRegion(UVRegionLower);

			NewWidget->Init(SlotData.Entry, SlotData.ItemDefinition, Brush, LowerSlotVertical);
			break;
		}
	case SuperiorSlotVertical:
		{
			FBox2d UVRegionSuperior(FVector2d(0.0, 0.0), FVector2d(1.0, 0.5));
			Brush.SetUVRegion(UVRegionSuperior);
							
			NewWidget->Init(SlotData.Entry, SlotData.ItemDefinition, Brush, SuperiorSlotVertical);
			break;
		}
	case UniqueSlot: { NewWidget->Init(SlotData.Entry,SlotData.ItemDefinition, Brush, UniqueSlot); break; }
	default: break;
	}
}

void UItemsPanelWidget::BindItemSlotDelegates(UItemSlotWidget* NewWidget)
{
	if (!IsValid(NewWidget)) return;
	NewWidget->OnItemDroppedPanelDelegate.BindUObject(this,&UItemsPanelWidget::HandleItemDropped);
	NewWidget->OnDragEnteredDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemEntered);
	NewWidget->OnDragLeavedDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemLeaved);
	NewWidget->OnDragCancelledDelegate.BindUObject(this,&UItemsPanelWidget::HandleDragCancelled);
	NewWidget->OnItemRowClickedDelegate.BindUObject(this, &UItemsPanelWidget::HandleSlotClicked);
	NewWidget->OnSlotMouseEnteredDelegate.BindUObject(this,&UItemsPanelWidget::HandleSlotHovered);
	NewWidget->OnSlotMouseLeavedDelegate.BindUObject(this,&UItemsPanelWidget::HandleSlotLeaved);
	NewWidget->OnNewDragOperation.BindUObject(this,&UItemsPanelWidget::HandleEquipmentEntered);
	// NewWidget->OnItemSlotMouseEnteredDelegate.BindUObject(this,&UItemsPanelWidget::HandleSlotHovered);
}

void UItemsPanelWidget::HandleEquipmentEntered(UItemSlotDragDrogOperation* InCurrentDragOperation)
{
	CurrentDragOperation = InCurrentDragOperation;
}

void UItemsPanelWidget::CreateSlotWidget(int32 Index,const FItemSlotData& SlotData)
{
	UItemSlotWidget* NewWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), ItemSlotWidgetClass);
	if (!IsValid(NewWidget)) return;

	if (SlotData.bIsEmpty)
	{
		NewWidget->EmptySlot();
		NewWidget->SetLinkedSlot(nullptr);
		BindItemSlotDelegates(NewWidget);
		AddItemToGrid(NewWidget, Index);
		return;
	}

	if (SlotData.Icon.IsNull()) return;

	FSlateBrush Brush;
	if (SlotData.Icon.IsValid())
	{
		Brush.SetResourceObject(SlotData.Icon.Get());
		InitializeSlotWidget(SlotData, NewWidget, Brush);
		BindItemSlotDelegates(NewWidget);
		AddItemToGrid(NewWidget, Index);

		// if (CurrentSelectedIndex != INDEX_NONE)
		// {
		// 	CurrentSelectedIndex = Index;
		// }
	}
	else
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

		Streamable.RequestAsyncLoad(SlotData.Icon.ToSoftObjectPath(),
	FStreamableDelegate::CreateLambda([this, SlotData,NewWidget,Index]()
			{
				if (UTexture2D* LoadedTexture = SlotData.Icon.Get())
				{
					FSlateBrush AsyncBrush;
					AsyncBrush.SetResourceObject(LoadedTexture);
					
					InitializeSlotWidget(SlotData, NewWidget, AsyncBrush);
					BindItemSlotDelegates(NewWidget);
					AddItemToGrid(NewWidget, Index);
					// if (CurrentSelectedIndex == INDEX_NONE)
					// {
					// 	CurrentSelectedIndex = Index;
					// }
				}
			})
		);
	}
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
	for (int32 i = 0; i < ItemsArray.Num(); i++)
	{
		if (ItemsArray[i].Size == SuperiorSlotVertical)
		{
			const int32 LowerIndex = i + MaxColumns;
			if (ItemsArray.IsValidIndex(LowerIndex))
			{
				UItemSlotWidget* SuperiorWidget = GetItemSlotbyIndex(i);
				UItemSlotWidget* LowerWidget    = GetItemSlotbyIndex(LowerIndex);
				if (IsValid(SuperiorWidget) && IsValid(LowerWidget))
				{
					SuperiorWidget->SetLinkedSlot(LowerWidget);
					LowerWidget->SetLinkedSlot(SuperiorWidget);
				}
			}
		}
	}

	SelectSlotAtIndex(CurrentSelectedIndex);
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

	SelectSlotAtIndex(0);
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
	SelectSlotAtIndex(0);
}

const FRPGInventoryEntry& UItemsPanelWidget::GetSelectedItem()
{
	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	
	const FItemSlotData& Selected = ItemsArray[CurrentSelectedIndex];

	return Selected.Entry; 
}



// -------------------------------------
//  Drag & Drop – Panel-side logic
// -------------------------------------

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

void UItemsPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemToolTipReference = CreateWidget<UItemToolTip>(GetOwningPlayer(), TooltipWidgetClass);
	ItemToolTipReference->AddToViewport(999); 
	ItemToolTipReference->SetVisibility(ESlateVisibility::Collapsed);

	LastHoveredIndex = INDEX_NONE;

	SubCategoryText->SetText(SubCategoryEditableText);
}

void UItemsPanelWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	SubCategoryText->SetText(SubCategoryEditableText);
}

void UItemsPanelWidget::ResolvePair(const TArray<FItemSlotData>& ItemsArray, int32 TargetIndex,
                                    ESlotSizeCategories DraggedSize, int32 MaxColumns, int32& OutSuperior, int32& OutLower)
{
	//  Given TargetIndex and DraggedSize,
	//  Determines the pair (SuperiorIndex, LowerIndex) that should fill.
	const FItemSlotData& TargetSlot = ItemsArray[TargetIndex];

	if (TargetSlot.Size == SuperiorSlotVertical)
	{
		OutSuperior = TargetIndex;
		OutLower    = TargetIndex + MaxColumns;
	}
	else if (TargetSlot.Size == LowerSlotVertical)
	{
		OutSuperior = TargetIndex - MaxColumns;
		OutLower    = TargetIndex;
	}
	else // empty
	{
		if (DraggedSize == SuperiorSlotVertical)
		{
			OutSuperior = TargetIndex;
			OutLower    = TargetIndex + MaxColumns;
		}
		else 
		{
			OutSuperior = TargetIndex - MaxColumns;
			OutLower    = TargetIndex;
		}
	}
}

void UItemsPanelWidget::HandleItemDropped(int32 DroppedIndex,int32 NewIndex)
{
	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (!ItemsArray.IsValidIndex(NewIndex) && ItemsArray[NewIndex].bIsEmpty) return;

	const FItemSlotData& DroppedSlot = ItemsArray[NewIndex];
		
	if (TryDropInNewSlot(DroppedIndex,NewIndex))
	{
		ResetCategory(CurrentCategoryTag);
		SelectSlotAtIndex(NewIndex);
	}
	else
	{
		HandleDragCancelled(DroppedSlot.Entry);
	}
}


void UItemsPanelWidget::HandleDraggedItemEntered(int32 DraggedIndex, int32 TargetIndex)
{
	if (!ItemsPanel) return;
	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;

	const bool bFromEquipment = (DraggedIndex == INDEX_NONE);
	
	FItemSlotData DraggedSlot;
	//From Equipment Slot
	if (bFromEquipment)
	{
		if (!IsValid(CurrentDragOperation)) return;
		
		DraggedSlot.Size = CurrentDragOperation->SlotSize;
		DraggedSlot.bIsEmpty = false;

		int32 TargetSuperiorIndex = INDEX_NONE;
		int32 TargetLowerIndex    = INDEX_NONE;

		ResolvePair(ItemsArray, TargetIndex, DraggedSlot.Size, MaxColumns, TargetSuperiorIndex, TargetLowerIndex);

		if (!ItemsArray.IsValidIndex(TargetSuperiorIndex) || !ItemsArray.IsValidIndex(TargetLowerIndex)) return;

		const FItemSlotData& TopSlot    = ItemsArray[TargetSuperiorIndex];
		const FItemSlotData& BottomSlot = ItemsArray[TargetLowerIndex];
		const FItemSlotData& TargetSlot = ItemsArray[TargetIndex];

		EDragOverResult Result = EDragOverResult::Invalid;

		if (TargetSlot.Size == UniqueSlot && !TargetSlot.bIsEmpty)
		{
			Result = EDragOverResult::Invalid;
		}
		else if (TopSlot.bIsEmpty && BottomSlot.bIsEmpty)
		{
			Result = EDragOverResult::Drop;
		}
		else if (TopSlot.bIsEmpty != BottomSlot.bIsEmpty)
		{
			Result = EDragOverResult::Invalid;
		}
		else if (TopSlot.Size == SuperiorSlotVertical || TopSlot.Size == LowerSlotVertical)
		{
			Result = EDragOverResult::Swap;
		}

		UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(TargetSuperiorIndex);
		UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(TargetLowerIndex);

		if (IsValid(TopWidget))
		{
			TopWidget->EnableDragOverPreview(Result);
		}
		if (IsValid(BottomWidget)) BottomWidget->EnableDragOverPreview(Result);
		CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(Result);

		return;
	}
	
	//From Grid panel Logic
	if (!ItemsArray.IsValidIndex(DraggedIndex) || !ItemsArray.IsValidIndex(TargetIndex)) return;
	DraggedSlot = ItemsArray[DraggedIndex];
	
 
	// Dragged Item Pair
	const int32 DraggedSuperiorIndex = (DraggedSlot.Size == SuperiorSlotVertical)
	                                     ? DraggedIndex
	                                     : DraggedIndex - MaxColumns;
	const int32 DraggedLowerIndex = DraggedSuperiorIndex + MaxColumns;

	// Detect Overlapings before Resolve Pair
	const int32 EmptyAboveTheTop = DraggedSuperiorIndex - MaxColumns;
	const int32 EmptyBelowTheLower     = DraggedLowerIndex    + MaxColumns;
 
	const bool bIsOwnSuperior    = (TargetIndex == DraggedSuperiorIndex);
	const bool bIsOwnLower       = (TargetIndex == DraggedLowerIndex);
	const bool bIsAboveOwn       = (TargetIndex == EmptyAboveTheTop);
	const bool bIsBelowOwn       = (TargetIndex == EmptyBelowTheLower);
 
	if (bIsOwnSuperior || bIsOwnLower)
	{
		// Hover over the item (Superior or Lower)
		UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(DraggedSuperiorIndex);
		UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(DraggedLowerIndex);
		if (IsValid(TopWidget))
		{
			TopWidget->EnableDragOverPreview(EDragOverResult::Swap);
			CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(EDragOverResult::Swap);
			//TopWidget->EnableDragOverResultIcon(EDragOverResult::Swap, DraggedSlot.Size);
		}
		if (IsValid(BottomWidget)) BottomWidget->EnableDragOverPreview(EDragOverResult::Drop);
		return;
	}
 
	if (bIsAboveOwn)
	{
		// Empty space just ABOVE the Superior
		const bool bCanDrop = ItemsArray[TargetIndex].bIsEmpty;
		const EDragOverResult Result = bCanDrop ? EDragOverResult::Drop : EDragOverResult::Invalid;
 
		UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(TargetIndex);           
		UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(DraggedSuperiorIndex);  
		if (IsValid(TopWidget))
		{
			TopWidget->EnableDragOverPreview(Result);
			//TopWidget->EnableDragOverResultIcon(Result, DraggedSlot.Size);
			CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(Result);
		}
		if (IsValid(BottomWidget)) BottomWidget->EnableDragOverPreview(Result);
		return;
	}
 
	if (bIsBelowOwn)
	{
		// Empty space BELOW the Lower
		const bool bCanDrop = ItemsArray[TargetIndex].bIsEmpty;
		const EDragOverResult Result = bCanDrop ? EDragOverResult::Drop : EDragOverResult::Invalid;
 
		UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(DraggedLowerIndex);  
		UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(TargetIndex);        
		if (IsValid(TopWidget))
		{
			TopWidget->EnableDragOverPreview(Result);
			//TopWidget->EnableDragOverResultIcon(Result, DraggedSlot.Size);
			CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(Result);
		}
		if (IsValid(BottomWidget)) BottomWidget->EnableDragOverPreview(Result);
		return;
	}
 
	// Resolve Pair
	int32 TargetSuperiorIndex = INDEX_NONE;
	int32 TargetLowerIndex    = INDEX_NONE;
	ResolvePair(ItemsArray, TargetIndex, DraggedSlot.Size, MaxColumns, TargetSuperiorIndex, TargetLowerIndex);
 
	if (!ItemsArray.IsValidIndex(TargetSuperiorIndex) || !ItemsArray.IsValidIndex(TargetLowerIndex)) return;
 
	const FItemSlotData& TargetSuperiorSlot = ItemsArray[TargetSuperiorIndex];
	const FItemSlotData& TargetLowerSlot    = ItemsArray[TargetLowerIndex];
	const FItemSlotData& TargetSlot         = ItemsArray[TargetIndex];
 
	EDragOverResult Result = EDragOverResult::Invalid;
 
	if (TargetSlot.Size == UniqueSlot && !TargetSlot.bIsEmpty)
	{
		Result = EDragOverResult::Invalid;
	}
	else if (TargetSuperiorSlot.bIsEmpty && TargetLowerSlot.bIsEmpty)
	{
		Result = EDragOverResult::Drop;
	}
	else if (TargetSuperiorSlot.bIsEmpty != TargetLowerSlot.bIsEmpty)
	{
		Result = EDragOverResult::Invalid;
	}
	else if (TargetSuperiorSlot.Size == SuperiorSlotVertical || TargetSuperiorSlot.Size == LowerSlotVertical)
	{
		Result = EDragOverResult::Swap;
	}
	else
	{
		Result = EDragOverResult::Invalid;
	}
 
	UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(TargetSuperiorIndex);
	UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(TargetLowerIndex);
	if (IsValid(TopWidget))
	{
		TopWidget->EnableDragOverPreview(Result);
		//TopWidget->EnableDragOverResultIcon(Result, DraggedSlot.Size);
		CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(Result);
	}
	if (IsValid(BottomWidget)) BottomWidget->EnableDragOverPreview(Result);
}
 
 

bool UItemsPanelWidget::TryDropInNewSlot(int32 FromIndex, int32 ToIndex)
{
	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return false;

	if (FromIndex == INDEX_NONE)
	{
		if (!CurrentDragOperation) return false;

		const ESlotSizeCategories FromSize = CurrentDragOperation->SlotSize;

		int32 ToSuperiorIndex = INDEX_NONE;
		int32 ToLowerIndex    = INDEX_NONE;

		ResolvePair(ItemsArray, ToIndex, FromSize, MaxColumns, ToSuperiorIndex, ToLowerIndex);

		if (!ItemsArray.IsValidIndex(ToSuperiorIndex) || !ItemsArray.IsValidIndex(ToLowerIndex)) return false;

		FItemSlotData& TopSlot    = ItemsArray[ToSuperiorIndex];
		FItemSlotData& BottomSlot = ItemsArray[ToLowerIndex];

		if (TopSlot.bIsEmpty && BottomSlot.bIsEmpty)
		{
			// Crear item en grid
			TopSlot.Entry = *CurrentDragOperation->ItemEntry;
			TopSlot.Icon = CurrentDragOperation->ItemDefinition.Icon.Get();
			TopSlot.Size = SuperiorSlotVertical;
			TopSlot.bIsEmpty = false;
			TopSlot.ItemDefinition = CurrentDragOperation->ItemDefinition;
			
			BottomSlot.Entry = *CurrentDragOperation->ItemEntry;
			BottomSlot.Icon = CurrentDragOperation->ItemDefinition.Icon.Get();
			BottomSlot.Size = LowerSlotVertical;
			BottomSlot.bIsEmpty = false;
			BottomSlot.ItemDefinition = CurrentDragOperation->ItemDefinition;
			
			//TopSlot.IconBrush =
			// UItemSlotWidget* TopWidget = GetItemSlotbyIndex(ToSuperiorIndex);
			// InitializeSlotWidget()
			
			// TopSlot = BuildSlotDataFromDrag(CurrentDragOperation, true);
			// BottomSlot = BuildSlotDataFromDrag(CurrentDragOperation, false);

			// Add equipment to inventory
			OnEquipmentDropped.ExecuteIfBound(CurrentDragOperation->ItemEntry->ItemTag,CurrentDragOperation->ItemEntry->ItemID);
			// Reset Equipment Slot
			CurrentDragOperation->SourceEquipmentSlot->EmptySlot();
			// Remove Equipment from Component and Character
			CurrentDragOperation->SourceEquipmentSlot->OnUnequipItem.ExecuteIfBound(*CurrentDragOperation->ItemEntry);
			
			return true;
		}

		return false;
	}
	
	if (!ItemsArray.IsValidIndex(FromIndex) || !ItemsArray.IsValidIndex(ToIndex)) return false;
 
	const ESlotSizeCategories FromSize = ItemsArray[FromIndex].Size;
	FItemSlotData& ToSlot = ItemsArray[ToIndex];
 
	// From is Unique
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
			ItemsArray.Swap(ToIndex, FromIndex);
			return true;
		}
		return false;
	}
 
	// From is Superior or Lower
	if (ToSlot.Size == UniqueSlot && !ToSlot.bIsEmpty) return false;
 
	const int32 FromSuperiorIndex = (FromSize == SuperiorSlotVertical)
	                                  ? FromIndex
	                                  : FromIndex - MaxColumns;
	const int32 FromLowerIndex = FromSuperiorIndex + MaxColumns;
	if (!ItemsArray.IsValidIndex(FromLowerIndex)) return false;
 
	// Detect overlaping
	const int32 VacioArribaDelSuperior = FromSuperiorIndex - MaxColumns;
	const int32 VacioAbajoDelLower     = FromLowerIndex    + MaxColumns;
 
	const bool bIsOwnSuperior = (ToIndex == FromSuperiorIndex);
	const bool bIsOwnLower    = (ToIndex == FromLowerIndex);
	const bool bIsAboveOwn    = (ToIndex == VacioArribaDelSuperior);
	const bool bIsBelowOwn    = (ToIndex == VacioAbajoDelLower);
 
	if (bIsOwnSuperior || bIsOwnLower)
	{
		// Self-drop
		return false;
	}
 
	if (bIsAboveOwn)
	{
		// Upward shift
		if (!ItemsArray[ToIndex].bIsEmpty) return false;
 
		FItemSlotData SavedSuperior = ItemsArray[FromSuperiorIndex];
		FItemSlotData SavedLower    = ItemsArray[FromLowerIndex];
		ResetSlotData(ItemsArray[FromSuperiorIndex]);
		ResetSlotData(ItemsArray[FromLowerIndex]);
		ItemsArray[ToIndex]             = SavedSuperior;
		ItemsArray[FromSuperiorIndex]   = SavedLower;
		return true;
	}
 
	if (bIsBelowOwn)
	{
		// Downward shift
		if (!ItemsArray[ToIndex].bIsEmpty) return false;
 
		FItemSlotData SavedSuperior = ItemsArray[FromSuperiorIndex];
		FItemSlotData SavedLower    = ItemsArray[FromLowerIndex];
		ResetSlotData(ItemsArray[FromSuperiorIndex]);
		ResetSlotData(ItemsArray[FromLowerIndex]);
		ItemsArray[FromLowerIndex] = SavedSuperior;
		ItemsArray[ToIndex]        = SavedLower;
		return true;
	}
 
	//No overlap
	int32 ToSuperiorIndex = INDEX_NONE;
	int32 ToLowerIndex    = INDEX_NONE;
	ResolvePair(ItemsArray, ToIndex, FromSize, MaxColumns, ToSuperiorIndex, ToLowerIndex);
 
	if (!ItemsArray.IsValidIndex(ToSuperiorIndex) || !ItemsArray.IsValidIndex(ToLowerIndex)) return false;
	if (ToSuperiorIndex == FromSuperiorIndex) return false;
 
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
 
	if (ToSuperiorSlot.bIsEmpty != ToLowerSlot.bIsEmpty) return false;
 
	if (ToSuperiorSlot.Size == SuperiorSlotVertical || ToSuperiorSlot.Size == LowerSlotVertical)
	{
		ItemsArray.Swap(FromSuperiorIndex, ToSuperiorIndex);
		ItemsArray.Swap(FromLowerIndex,    ToLowerIndex);
		return true;
	}
 
	return false;
}
 

void UItemsPanelWidget::HandleDraggedItemLeaved(int32 DraggedIndex, int32 TargetIndex)
{
	const TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;
	
	if (DraggedIndex == INDEX_NONE)
	{
		if (!CurrentDragOperation) return;

		FItemSlotData DraggedSlot;
		DraggedSlot.Size = CurrentDragOperation->SlotSize;

		int32 CleanTopIndex    = INDEX_NONE;
		int32 CleanBottomIndex = INDEX_NONE;

		ResolvePair(ItemsArray, TargetIndex, DraggedSlot.Size, MaxColumns, CleanTopIndex, CleanBottomIndex);

		if (CleanTopIndex == INDEX_NONE) return;

		UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(CleanTopIndex);
		UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(CleanBottomIndex);

		if (IsValid(TopWidget))
		{
			TopWidget->DisableDragOverPreview();
			//TopWidget->DisableDragOverResultIcon();
			CurrentDragOperation->ItemDraggedIconWidget->DisableDragOverResultIcon();
		}
		if (IsValid(BottomWidget)) BottomWidget->DisableDragOverPreview();

		return;
	}
	
	
	if (!ItemsArray.IsValidIndex(DraggedIndex) || !ItemsArray.IsValidIndex(TargetIndex)) return;
 
	const FItemSlotData& DraggedSlot = ItemsArray[DraggedIndex];
 
	const int32 DraggedSuperiorIndex = (DraggedSlot.Size == SuperiorSlotVertical)
	                                     ? DraggedIndex
	                                     : DraggedIndex - MaxColumns;
	const int32 DraggedLowerIndex    = DraggedSuperiorIndex + MaxColumns;
 
	const int32 VacioArribaDelSuperior = DraggedSuperiorIndex - MaxColumns;
	const int32 VacioAbajoDelLower     = DraggedLowerIndex    + MaxColumns;
 
	const bool bIsOwnSuperior = (TargetIndex == DraggedSuperiorIndex);
	const bool bIsOwnLower    = (TargetIndex == DraggedLowerIndex);
	const bool bIsAboveOwn    = (TargetIndex == VacioArribaDelSuperior);
	const bool bIsBelowOwn    = (TargetIndex == VacioAbajoDelLower);
 
	int32 CleanTopIndex    = INDEX_NONE;
	int32 CleanBottomIndex = INDEX_NONE;
 
	if (bIsOwnSuperior || bIsOwnLower)
	{
		CleanTopIndex    = DraggedSuperiorIndex;
		CleanBottomIndex = DraggedLowerIndex;
	}
	else if (bIsAboveOwn)
	{
		CleanTopIndex    = TargetIndex;          
		CleanBottomIndex = DraggedSuperiorIndex;  
	}
	else if (bIsBelowOwn)
	{
		CleanTopIndex    = DraggedLowerIndex;  
		CleanBottomIndex = TargetIndex;        
	}
	else
	{
		// no overlap
		ResolvePair(ItemsArray, TargetIndex, DraggedSlot.Size, MaxColumns, CleanTopIndex, CleanBottomIndex);
	}
 
	if (CleanTopIndex == INDEX_NONE) return;
 
	UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(CleanTopIndex);
	UItemSlotWidget* BottomWidget = ItemsArray.IsValidIndex(CleanBottomIndex)
	                                  ? GetItemSlotbyIndex(CleanBottomIndex)
	                                  : nullptr;
 
	if (IsValid(TopWidget))
	{
		TopWidget->DisableDragOverPreview();
		CurrentDragOperation->ItemDraggedIconWidget->DisableDragOverResultIcon();
	}
	if (IsValid(BottomWidget)) BottomWidget->DisableDragOverPreview();
}
 

void UItemsPanelWidget::HandleDragCancelled(const FRPGInventoryEntry& Entry)
{
	const TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (ItemsArray.IsEmpty()) return;
	
	int32 LastEnterIndex = FindGridIndexByItemID(Entry.ItemID,Entry.ItemTag);
	
	if (!ItemsArray.IsValidIndex(LastEnterIndex)) return;
 
	const FItemSlotData& LastEnterSlot = ItemsArray[LastEnterIndex];
	if (LastEnterSlot.Size == UniqueSlot) return;
	
	const ESlotSizeCategories DraggedSizeGuess =
		(LastEnterSlot.Size == LowerSlotVertical || LastEnterSlot.Size == SuperiorSlotVertical)
		? LastEnterSlot.Size.GetValue()
		: SuperiorSlotVertical;
 
	int32 TargetSuperiorIndex = INDEX_NONE;
	int32 TargetLowerIndex    = INDEX_NONE;
	ResolvePair(ItemsArray, LastEnterIndex, DraggedSizeGuess, MaxColumns, TargetSuperiorIndex, TargetLowerIndex);
 
	UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(TargetSuperiorIndex);
	UItemSlotWidget* BottomWidget = ItemsArray.IsValidIndex(TargetLowerIndex)
	                                  ? GetItemSlotbyIndex(TargetLowerIndex)
	                                  : nullptr;
 
	if (IsValid(TopWidget))    { TopWidget->DisableDragOverPreview(); CurrentDragOperation->ItemDraggedIconWidget->DisableDragOverResultIcon();/*TopWidget->DisableDragOverResultIcon();*/ }
	if (IsValid(BottomWidget))   BottomWidget->DisableDragOverPreview();
}

void UItemsPanelWidget::HandleSlotHovered(UBaseInventorySlot* BaseSlot)
{
	if (!IsValid(BaseSlot)) return;
	UItemSlotWidget* ItemSlotWidget = Cast<UItemSlotWidget>(BaseSlot);
    if (!IsValid(ItemSlotWidget) || !IsValid(ItemToolTipReference)) return;

    ESlotSizeCategories Size = ItemSlotWidget->GetCurrentSlotSize();
    int32 SlotGridIndex = ItemSlotWidget->GetGridIndex();

    // GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,FString::Printf(TEXT("Hovered Current: %i"), SlotGridIndex));
    // GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,FString::Printf(TEXT("Hovered Last: %i"), LastHoveredIndex)); // fix

    // Ignorar si ya estamos sobre este slot o su compañero
    if (LastHoveredIndex == SlotGridIndex) return;
    if (Size == SuperiorSlotVertical && LastHoveredIndex == SlotGridIndex + MaxColumns) return;
    if (Size == LowerSlotVertical    && LastHoveredIndex == SlotGridIndex - MaxColumns) return;

    // Actualizar LastHoveredIndex SOLO si vamos a procesar este slot
    LastHoveredIndex = SlotGridIndex;

    TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
    if (!ItemsArray.IsValidIndex(LastHoveredIndex)) return;

    // Slot vacío: ocultar tooltip pero SÍ actualizar LastHoveredIndex (ya lo hicimos arriba)
    if (ItemSlotWidget->IsEmpty())
    {
        ItemToolTipReference->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    const FItemSlotData& ItemSlot = ItemsArray[LastHoveredIndex];
    ItemToolTipReference->SetData(ItemSlot.ItemDefinition);

    FGeometry SlotGeometry = ItemSlotWidget->GetCachedGeometry();
    if (Size == SuperiorSlotVertical)
    {
        UItemSlotWidget* LowerSlot = GetItemSlotbyIndex(LastHoveredIndex + MaxColumns);
        if (IsValid(LowerSlot)) SlotGeometry = LowerSlot->GetCachedGeometry();
    }

    FVector2D AbsPos  = SlotGeometry.GetAbsolutePosition();
    FVector2D AbsSize = SlotGeometry.GetAbsoluteSize();

    FVector2D PixelPosition;
    FVector2D ViewportPosition;
    USlateBlueprintLibrary::AbsoluteToViewport(
        GetWorld(),
        FVector2D(AbsPos.X + AbsSize.X, AbsPos.Y + AbsSize.Y),
        PixelPosition,
        ViewportPosition
    );

    ItemToolTipReference->SetPositionInViewport(ViewportPosition, false);
    ItemToolTipReference->SetVisibility(ESlateVisibility::Visible);
}

void UItemsPanelWidget::HandleSlotLeaved(UBaseInventorySlot* BaseSlot)
{
	if (!IsValid(BaseSlot)) return;
	UItemSlotWidget* ItemSlotWidget = Cast<UItemSlotWidget>(BaseSlot);
	if (!IsValid(ItemSlotWidget)) return;
	
	ESlotSizeCategories Size = ItemSlotWidget->GetCurrentSlotSize();
	int32 SlotGridIndex = ItemSlotWidget->GetGridIndex();

	// Calcular el índice superior del par que está dejando
	int32 LeavingPairSuperior = INDEX_NONE;
	if      (Size == SuperiorSlotVertical) LeavingPairSuperior = SlotGridIndex;
	else if (Size == LowerSlotVertical)    LeavingPairSuperior = SlotGridIndex - MaxColumns;
	else                                   LeavingPairSuperior = SlotGridIndex; // UniqueSlot

	// Calcular el índice superior del par que está activo (LastHoveredIndex)
	int32 ActivePairSuperior = INDEX_NONE;
	if (LastHoveredIndex != INDEX_NONE)
	{
		const TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
		if (ItemsArray.IsValidIndex(LastHoveredIndex))
		{
			ESlotSizeCategories ActiveSize = ItemsArray[LastHoveredIndex].Size;
			if      (ActiveSize == SuperiorSlotVertical) ActivePairSuperior = LastHoveredIndex;
			else if (ActiveSize == LowerSlotVertical)    ActivePairSuperior = LastHoveredIndex - MaxColumns;
			else                                         ActivePairSuperior = LastHoveredIndex;
		}
	}

	// Solo ocultar si el par que abandona ES el par activo
	// Si ya estamos en otro par distinto, este leave es obsoleto
	if (LeavingPairSuperior != ActivePairSuperior) return;

	// Verificar que el compañero no está siendo entrado ahora mismo
	// (el CancelLeaveTimer en NativeOnMouseEnter ya maneja esto, pero por si acaso)
	LastHoveredIndex = INDEX_NONE;
	ItemToolTipReference->SetVisibility(ESlateVisibility::Collapsed);
}


void UItemsPanelWidget::HandleSlotClicked(int32 ClickedIndex)
{
	TArray<FItemSlotData>& ItemsArray = *CategoryItemsMap.Find(CurrentCategoryTag);
	if (!ItemsArray.IsValidIndex(ClickedIndex)) return;
	if (ItemsArray[ClickedIndex].bIsEmpty) return;

	SelectSlotAtIndex(ClickedIndex);
}

