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
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Libraries/RPGUIStatics.h"
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

	for (int i = 0; i < ItemsArray.Num();i++)
	{
		FItemSlotData& ItemSlotData = ItemsArray[i];
		if (!ItemSlotData.bIsEmpty && ItemSlotData.Entry.ItemID == ItemID)
		{
			if (ItemSlotData.Size == ESlotSizeCategories::SuperiorSlotVertical)
			{
				ResetSlotData(ItemsArray[i + MaxColumns]);
			}
			if (ItemSlotData.Size == ESlotSizeCategories::LowerSlotVertical)
			{
				ResetSlotData(ItemsArray[i - MaxColumns]);
			}
			ResetSlotData(ItemSlotData);
			ItemRemoved = true;
			break;
		}
	}
	if (!ItemRemoved) return;
	ResetItemsArray();
}

int32 UItemsPanelWidget::FindGridIndexByItemID(const int64 ItemID,FGameplayTag ItemTag)
{
	if (ItemsArray.IsEmpty()) return -1;
	for (int32 i = 0; i < ItemsArray.Num(); i++)
	{
		if (!(ItemsArray)[i].bIsEmpty &&
			(ItemsArray)[i].Entry.ItemID == ItemID)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

void UItemsPanelWidget::UpdateItemSlot(const FRPGInventoryEntry& Entry)
{
	if (ItemsArray.IsEmpty()) return;

	for (FItemSlotData& ItemSlotData : ItemsArray)
	{
		if (!ItemSlotData.bIsEmpty && ItemSlotData.Entry.ItemID == Entry.ItemID)
		{
			ItemSlotData.Entry = Entry;
			break;
		}
	}

	ResetItemsArray();
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
	//Already exist
	if (FindGridIndexByItemID(Entry.ItemID, ItemDefinition.ItemTag) != INDEX_NONE) return;
	
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
	ItemsArray[FreeIndex].Size = ESlotSizeCategories::UniqueSlot;
	ItemsArray[FreeIndex].ItemDefinition = ItemDefinition;
	
	FSlateBrush Brush;
	Brush.SetResourceObject(ItemDefinition.Icon.Get());
	
	if (ItemDefinition.CategoryTag.MatchesTag(RPGInventoryTags::ItemsCategory::Equipment) && ItemDefinition.SlotsSize == 2)
	{   //Slot Size == 2
		ItemsArray[FreeIndex].Size = ESlotSizeCategories::SuperiorSlotVertical;
		ItemsArray[FreeIndex + MaxColumns].Entry = Entry;
		ItemsArray[FreeIndex + MaxColumns].Icon = ItemDefinition.Icon;
		ItemsArray[FreeIndex + MaxColumns].bIsEmpty = false;
		ItemsArray[FreeIndex + MaxColumns].Size = ESlotSizeCategories::LowerSlotVertical;
		ItemsArray[FreeIndex + MaxColumns].ItemDefinition = ItemDefinition;
	}

	CurrentSelectedIndex = FreeIndex;
	
	if (bResetPanel)
	{
		ResetItemsArray();
	}

	SelectCurrentIndexSlot(CurrentSelectedIndex);
}

void UItemsPanelWidget::ClearPanel()
{
	if (!IsValid(ItemsPanel)) return;
	
	ItemsPanel->ClearChildren();	
}

void UItemsPanelWidget::SelectCurrentIndexSlot(int32 NewIndex)
{
	// Deselect old last selected
	if (CurrentSelectedIndex != NewIndex)
	{
		DeselectCurrentSlot();
	}

	CurrentSelectedIndex = NewIndex;
	
	if (!ItemsArray.IsValidIndex(CurrentSelectedIndex)) return;
	
	UItemSlotWidget* SlotWidget = GetItemSlotbyIndex(CurrentSelectedIndex);
	if (IsValid(SlotWidget) && !SlotWidget->IsEmpty()) SlotWidget->StartSelectedAnimation();
	

	const FItemSlotData& SlotData = ItemsArray[CurrentSelectedIndex];
	OnSelectItemDelegate.ExecuteIfBound(CurrentSelectedIndex);
	
	if (SlotData.Size == ESlotSizeCategories::SuperiorSlotVertical)
	{
		if (!ItemsArray.IsValidIndex(CurrentSelectedIndex + MaxColumns)) return;
		UItemSlotWidget* LowerWidget = GetItemSlotbyIndex(CurrentSelectedIndex + MaxColumns);
		if (IsValid(LowerWidget) && !LowerWidget->IsEmpty()) LowerWidget->StartSelectedAnimation();
	}
	else if (SlotData.Size == ESlotSizeCategories::LowerSlotVertical)
	{
		if (!ItemsArray.IsValidIndex(CurrentSelectedIndex - MaxColumns)) return;
		UItemSlotWidget* SuperiorWidget = GetItemSlotbyIndex(CurrentSelectedIndex - MaxColumns);
		if (IsValid(SuperiorWidget) && !SuperiorWidget->IsEmpty()) SuperiorWidget->StartSelectedAnimation();
	}
}

void UItemsPanelWidget::DeselectCurrentSlot()
{
	if (!ItemsArray.IsValidIndex(CurrentSelectedIndex))
	{
		return;
	}

	UItemSlotWidget* SlotWidget = GetItemSlotbyIndex(CurrentSelectedIndex);
	if (IsValid(SlotWidget)) SlotWidget->StopSelectedAnimation();

	const FItemSlotData& SlotData = ItemsArray[CurrentSelectedIndex];
	if (SlotData.Size == ESlotSizeCategories::SuperiorSlotVertical)
	{
		UItemSlotWidget* LowerWidget = GetItemSlotbyIndex(CurrentSelectedIndex + MaxColumns);
		if (IsValid(LowerWidget)) LowerWidget->StopSelectedAnimation();
	}
	else if (SlotData.Size == ESlotSizeCategories::LowerSlotVertical)
	{
		UItemSlotWidget* SuperiorWidget = GetItemSlotbyIndex(CurrentSelectedIndex - MaxColumns);
		if (IsValid(SuperiorWidget)) SuperiorWidget->StopSelectedAnimation();
	}
}

void UItemsPanelWidget::AddEmptySlots()
{
	while (ItemsArray.Num() < NUM_INITIAL_EMPTY_SLOTS)
	{
		FItemSlotData NewSlot;
		NewSlot.bIsEmpty = true;
		ItemsArray.Add(NewSlot);
	}
	
	ResetItemsArray();
}

void UItemsPanelWidget::ResetSlotData(FItemSlotData& ItemSlotData)
{
	ItemSlotData.Entry = FRPGInventoryEntry();
	ItemSlotData.Icon = nullptr;
	ItemSlotData.Size = ESlotSizeCategories::UniqueSlot;
	ItemSlotData.bIsEmpty = true;
	ItemSlotData.ItemDefinition = FMasterItemDefinition();
	ItemSlotData.IconBrush = FSlateBrush();
}

void UItemsPanelWidget::InitializeSlotWidget(const FItemSlotData& SlotData, UItemSlotWidget* NewWidget, FSlateBrush Brush)
{
	switch (SlotData.Size)
	{
	case ESlotSizeCategories::LowerSlotVertical:
		{
			FBox2d UVRegionLower(FVector2d(0.0, 0.5), FVector2d(1.0, 1.0));
			Brush.SetUVRegion(UVRegionLower);

			NewWidget->Init(SlotData.Entry, SlotData.ItemDefinition, Brush, ESlotSizeCategories::LowerSlotVertical);
			break;
		}
	case ESlotSizeCategories::SuperiorSlotVertical:
		{
			FBox2d UVRegionSuperior(FVector2d(0.0, 0.0), FVector2d(1.0, 0.5));
			Brush.SetUVRegion(UVRegionSuperior);
							
			NewWidget->Init(SlotData.Entry, SlotData.ItemDefinition, Brush, ESlotSizeCategories::SuperiorSlotVertical);
			break;
		}
	case ESlotSizeCategories::UniqueSlot: { NewWidget->Init(SlotData.Entry,SlotData.ItemDefinition, Brush, ESlotSizeCategories::UniqueSlot); break; }
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
}

void UItemsPanelWidget::HandleEquipmentEntered(UItemSlotDragDrogOperation* InCurrentDragOperation)
{
	CurrentDragOperation = InCurrentDragOperation;
}

void UItemsPanelWidget::CreateSlotWidget(int32 Index,const FItemSlotData& SlotData)
{
	UItemSlotWidget* NewWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), ItemSlotWidgetClass);
	if (!IsValid(NewWidget)) return;

	NewWidget->SetAcceptedSubCategory(SubCategoryTag);

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
				}
			})
		);
	}
}

void UItemsPanelWidget::ResetItemsArray()
{
	if (!IsValid(ItemsPanel)) return;
	//clear panel
	ItemsPanel->ClearChildren();
	
	if (ItemsArray.IsEmpty()) return;

	for (int32 i = 0; i < ItemsArray.Num(); i++)
	{
		const FItemSlotData& SlotData = ItemsArray[i];

		CreateSlotWidget(i,SlotData);
	}
	for (int32 i = 0; i < ItemsArray.Num(); i++)
	{
		if (ItemsArray[i].Size == ESlotSizeCategories::SuperiorSlotVertical)
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
}

void UItemsPanelWidget::SortItemsQuicly()
{
	if (ItemsArray.IsEmpty()) return;
	
	TArray<FLogicalItem> LogicalItems;

	for (int32 i = 0; i < ItemsArray.Num(); i++)
	{
		const FItemSlotData& ItemSlot = ItemsArray[i];

		if (ItemSlot.bIsEmpty) continue;

		// get item root
		if (ItemSlot.Size == ESlotSizeCategories::LowerSlotVertical) continue;

		FLogicalItem Item;
		Item.Entry = ItemSlot.Entry;
		Item.ItemDefinition = ItemSlot.ItemDefinition;
		Item.Size = (ItemSlot.Size == ESlotSizeCategories::SuperiorSlotVertical) ? 2 : 1;

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
		ResetSlotData(ItemSlot);
	}

	//Add sorted slots
	for (const FLogicalItem& Item : LogicalItems)
	{
		AddItemSlot(Item.Entry, Item.ItemDefinition,false);
	}

	ResetItemsArray();

	SelectCurrentIndexSlot(0);
}

void UItemsPanelWidget::SortItemsBy(EItemSortType SortType)
{
	if (ItemsArray.IsEmpty()) return;
	
	TArray<FItemSlotData> SortedItems;

	for (const FItemSlotData& ItemSlot : ItemsArray)
	{
		if (ItemSlot.bIsEmpty) continue;
		if (ItemSlot.Size == ESlotSizeCategories::LowerSlotVertical) continue; //Skip lower and empties slots
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
				   int32 RarityA = URPGUIStatics::GetRarityTagPriority(GetWorld(),A.ItemDefinition.RarityTag);
				   int32 RarityB = URPGUIStatics::GetRarityTagPriority(GetWorld(),B.ItemDefinition.RarityTag);

				   if (RarityA != RarityB)
				   {
				   		return RarityA < RarityB; // Menor índice = mayor rareza
				   }
					int32 TypeA = URPGUIStatics::GetTypePriorityInCategory(GetWorld(),A.ItemDefinition.SubcategoryTag,A.ItemDefinition.TypeTag);
				    int32 TypeB = URPGUIStatics::GetTypePriorityInCategory(GetWorld(),B.ItemDefinition.SubcategoryTag,B.ItemDefinition.TypeTag);
					return TypeA < TypeB;
			    });
				break;
			}
		case EItemSortType::Type :
			{
				SortedItems.Sort([&](const FItemSlotData& A, const FItemSlotData& B)
				{
					int32 TypeA = URPGUIStatics::GetTypePriorityInCategory(GetWorld(),A.ItemDefinition.SubcategoryTag,A.ItemDefinition.TypeTag);
					int32 TypeB = URPGUIStatics::GetTypePriorityInCategory(GetWorld(),B.ItemDefinition.SubcategoryTag,B.ItemDefinition.TypeTag);

					if (TypeA != TypeB)
					{
						return TypeA < TypeB;
					}

					int32 RarityA = URPGUIStatics::GetRarityTagPriority(GetWorld(),A.ItemDefinition.RarityTag);
					int32 RarityB = URPGUIStatics::GetRarityTagPriority(GetWorld(),B.ItemDefinition.RarityTag);
					return RarityA < RarityB;
				});
			
				break;
			}
		default: break;
	}
	// Clean
	for (FItemSlotData& ItemSlot : ItemsArray)
	{
		ResetSlotData(ItemSlot);
	}

	// Re-add sorted
	for (const FItemSlotData& Item : SortedItems)
	{
		AddItemSlot(Item.Entry, Item.ItemDefinition,false);
	}

	ResetItemsArray();
	
	SelectCurrentIndexSlot(0);
}

const FRPGInventoryEntry* UItemsPanelWidget::GetSelectedItem()
{
	if (!ItemsArray.IsValidIndex(CurrentSelectedIndex)) return nullptr;
	
	const FItemSlotData& Selected = ItemsArray[CurrentSelectedIndex];

	return &Selected.Entry; 
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

// void UItemsPanelWidget::ResolvePair(const TArray<FItemSlotData>& ItemsArray, int32 TargetIndex,
//                                     ESlotSizeCategories DraggedSize, int32 MaxColumns, int32& OutSuperior, int32& OutLower)
// {
// 	 // Given TargetIndex and DraggedSize,
// 	 // Determines the pair (SuperiorIndex, LowerIndex) that should fill.
// 	const FItemSlotData& TargetSlot = ItemsArray[TargetIndex];
// 	
// 	if (TargetSlot.Size == ESlotSizeCategories::SuperiorSlotVertical)
// 	{
// 		OutSuperior = TargetIndex;
// 		OutLower    = TargetIndex + MaxColumns;
// 	}
// 	else if (TargetSlot.Size == ESlotSizeCategories::LowerSlotVertical)
// 	{
// 		OutSuperior = TargetIndex;
// 		OutLower    = TargetIndex + MaxColumns;
// 	}
// 	else // empty
// 	{
// 		// const int32 PotentialSuperior = TargetIndex - MaxColumns;
//   //       
// 		// if (PotentialSuperior < 0)
// 		// {
// 		// 	// Can't go up, force downward pair instead
// 		// 	OutSuperior = TargetIndex;
// 		// 	OutLower    = TargetIndex + MaxColumns;
// 		// }
// 		// else
// 		// {
// 		// 	OutSuperior = PotentialSuperior;
// 		// 	OutLower    = TargetIndex;
// 		// }
// 		OutSuperior = TargetIndex;
// 		OutLower    = TargetIndex + MaxColumns;
// 	}
// }

void UItemsPanelWidget::HandleItemDropped(int32 DroppedIndex,int32 NewIndex,EDragOverResult SubCategoryResult)
{
	if (!ItemsArray.IsValidIndex(NewIndex) && ItemsArray[NewIndex].bIsEmpty) return;

	const ESlotSizeCategories DraggedSize = (DroppedIndex == INDEX_NONE && IsValid(CurrentDragOperation))
	   ? CurrentDragOperation->SlotSize
	   : ItemsArray[DroppedIndex].Size;
	
	const FItemSlotData& DroppedSlot = ItemsArray[NewIndex];
		
	if (SubCategoryResult == EDragOverResult::Invalid)
	{
		HandleDragCancelled(NewIndex, DroppedIndex, DraggedSize);
		return;
	}
	
	if (TryDropInNewSlot(DroppedIndex,NewIndex))
	{
		ResetItemsArray();
		SelectCurrentIndexSlot(NewIndex);
	}
	else
	{
		HandleDragCancelled(NewIndex, DroppedIndex, DraggedSize);
	}
}


void UItemsPanelWidget::HandleDraggedItemEntered(int32 DraggedIndex, int32 TargetIndex,EDragOverResult SubCategoryResult)
{
	if (!ItemsPanel) return;
	if (ItemsArray.IsEmpty()) return;

	const bool bFromEquipment = (DraggedIndex == INDEX_NONE);
	
	FItemSlotData DraggedSlot;
	int32 TargetSuperiorIndex = TargetIndex;
	int32 TargetLowerIndex    = TargetIndex + MaxColumns;
	
	//From Equipment Slot
	if (bFromEquipment)
	{
		if (!IsValid(CurrentDragOperation)) return;
		
		DraggedSlot.Size = CurrentDragOperation->SlotSize;
		DraggedSlot.bIsEmpty = false;

		//ResolvePair(ItemsArray, TargetIndex, DraggedSlot.Size, MaxColumns, TargetSuperiorIndex, TargetLowerIndex);
	
		if (!ItemsArray.IsValidIndex(TargetSuperiorIndex) || !ItemsArray.IsValidIndex(TargetLowerIndex)) return;

		if (UItemSlotWidget* TopWidget = GetItemSlotbyIndex(TargetSuperiorIndex))
			TopWidget->DisableDragOverPreview();
		if (UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(TargetLowerIndex))
			BottomWidget->DisableDragOverPreview();
		CurrentDragOperation->ItemDraggedIconWidget->DisableDragOverResultIcon();
		
		//Subcategory invalid
		if (SubCategoryResult == EDragOverResult::Invalid)
		{
			if (UItemSlotWidget* TopWidget = GetItemSlotbyIndex(TargetSuperiorIndex))
				TopWidget->EnableDragOverPreview(EDragOverResult::Invalid);
			if (UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(TargetLowerIndex))
				BottomWidget->EnableDragOverPreview(EDragOverResult::Invalid);
			CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(EDragOverResult::Invalid);
			return;
		}
		
		const FItemSlotData& TopSlot    = ItemsArray[TargetSuperiorIndex];
		const FItemSlotData& BottomSlot = ItemsArray[TargetLowerIndex];
		const FItemSlotData& TargetSlot = ItemsArray[TargetIndex];

		EDragOverResult Result = EDragOverResult::Invalid;

		if (TargetSlot.Size == ESlotSizeCategories::UniqueSlot && !TargetSlot.bIsEmpty)
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
		else if (TopSlot.Size == ESlotSizeCategories::SuperiorSlotVertical || TopSlot.Size == ESlotSizeCategories::LowerSlotVertical)
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
	const int32 DraggedSuperiorIndex = (DraggedSlot.Size == ESlotSizeCategories::SuperiorSlotVertical)
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

	if (UItemSlotWidget* TopWidget = GetItemSlotbyIndex(DraggedSuperiorIndex))
		TopWidget->DisableDragOverPreview();
	if (UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(DraggedLowerIndex))
		BottomWidget->DisableDragOverPreview();
	CurrentDragOperation->ItemDraggedIconWidget->DisableDragOverResultIcon();
	
	if (SubCategoryResult == EDragOverResult::Invalid)
	{
		// int32 TargetSuperiorIndex = INDEX_NONE;
		// int32 TargetLowerIndex    = INDEX_NONE;
		//ResolvePair(ItemsArray, TargetIndex, DraggedSlot.Size, MaxColumns, TargetSuperiorIndex, TargetLowerIndex);
		
		if (UItemSlotWidget* TopWidget = GetItemSlotbyIndex(TargetSuperiorIndex))
			TopWidget->EnableDragOverPreview(EDragOverResult::Invalid);
		if (UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(TargetLowerIndex))
			BottomWidget->EnableDragOverPreview(EDragOverResult::Invalid);
		CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(EDragOverResult::Invalid);
		return;
	}
	
	if (bIsOwnSuperior || bIsOwnLower)
	{
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Green,FString::Printf(TEXT("bIsOwnSuperior || bIsOwnLower: %d"),DraggedLowerIndex));
		// Hover over the item (Superior or Lower)
		UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(DraggedSuperiorIndex);
		UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(DraggedLowerIndex);
		if (IsValid(TopWidget))
		{
			TopWidget->EnableDragOverPreview(EDragOverResult::Swap);
			CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(EDragOverResult::Swap);
		}
		if (IsValid(BottomWidget)) BottomWidget->EnableDragOverPreview(EDragOverResult::Drop);
		return;
	}
 
	if (bIsAboveOwn)
	{
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple,FString::Printf(TEXT("bIsAboveOwn: %d"),DraggedLowerIndex));
		// Empty space just ABOVE the Superior
		const bool bCanDrop = ItemsArray[TargetIndex].bIsEmpty;
		const EDragOverResult Result = bCanDrop ? EDragOverResult::Drop : EDragOverResult::Invalid;
 
		UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(TargetIndex);           
		UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(DraggedSuperiorIndex);  
		if (IsValid(TopWidget))
		{
			TopWidget->EnableDragOverPreview(Result);
			CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(Result);
		}
		if (IsValid(BottomWidget)) BottomWidget->EnableDragOverPreview(Result);
		return;
	}
 
	if (bIsBelowOwn)
	{
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Magenta,FString::Printf(TEXT("DraggedLower: %d"),DraggedLowerIndex));
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Magenta,FString::Printf(TEXT("TargetIndex: %d"),TargetIndex));
		// Empty space BELOW the Lower
		const bool bCanDrop = ItemsArray[TargetIndex].bIsEmpty;
		const EDragOverResult Result = bCanDrop ? EDragOverResult::Drop : EDragOverResult::Invalid;
 
		UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(DraggedLowerIndex);  
		UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(TargetIndex);        
		if (IsValid(TopWidget))
		{
			TopWidget->EnableDragOverPreview(Result);
			CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(Result);
		}
		if (IsValid(BottomWidget)) BottomWidget->EnableDragOverPreview(Result);
		return;
	}
 
	// Resolve Pair
	// int32 TargetSuperiorIndex = INDEX_NONE;
	// int32 TargetLowerIndex    = INDEX_NONE;
	// ResolvePair(ItemsArray, TargetIndex, DraggedSlot.Size, MaxColumns, TargetSuperiorIndex, TargetLowerIndex);
 
	if (!ItemsArray.IsValidIndex(TargetSuperiorIndex) || !ItemsArray.IsValidIndex(TargetLowerIndex)) return;
 
	const FItemSlotData& TargetSuperiorSlot = ItemsArray[TargetSuperiorIndex];
	const FItemSlotData& TargetLowerSlot    = ItemsArray[TargetLowerIndex];
	const FItemSlotData& TargetSlot         = ItemsArray[TargetIndex];
 
	EDragOverResult Result = EDragOverResult::Invalid;
 
	if (TargetSlot.Size == ESlotSizeCategories::UniqueSlot && !TargetSlot.bIsEmpty)
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
	else if (TargetSuperiorSlot.Size == ESlotSizeCategories::SuperiorSlotVertical || TargetSuperiorSlot.Size == ESlotSizeCategories::LowerSlotVertical)
	{
		Result = EDragOverResult::Swap;
	}
	else
	{
		Result = EDragOverResult::Invalid;
	}
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,FString::Printf(TEXT("targetSuperior: %d"),TargetSuperiorIndex));
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,FString::Printf(TEXT("Target Lower: %d"),TargetLowerIndex));
	UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(TargetSuperiorIndex);
	UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(TargetLowerIndex);
	if (IsValid(TopWidget))
	{
		TopWidget->EnableDragOverPreview(Result);
		CurrentDragOperation->ItemDraggedIconWidget->EnableDragOverResultIcon(Result);
	}
	if (IsValid(BottomWidget)) BottomWidget->EnableDragOverPreview(Result);
}
 
 

bool UItemsPanelWidget::TryDropInNewSlot(int32 FromIndex, int32 ToIndex)
{
	if (ItemsArray.IsEmpty()) return false;

	if (FromIndex == INDEX_NONE)
	{
		if (!CurrentDragOperation) return false;

		const ESlotSizeCategories FromSize = CurrentDragOperation->SlotSize;
		if (FromSize == ESlotSizeCategories::UniqueSlot)
		{
			FItemSlotData& ToSlot    = ItemsArray[ToIndex];
			ToSlot.Entry = *CurrentDragOperation->ItemEntry;
			ToSlot.Icon = CurrentDragOperation->ItemDefinition.Icon.Get();
			ToSlot.Size = ESlotSizeCategories::UniqueSlot;
			ToSlot.bIsEmpty = false;
			ToSlot.ItemDefinition = CurrentDragOperation->ItemDefinition;

			// Add equipment to inventory
			OnEquipmentDropped.ExecuteIfBound(CurrentDragOperation->ItemEntry->ItemTag,CurrentDragOperation->ItemEntry->ItemID);
			// Reset Equipment Slot
			CurrentDragOperation->SourceEquipmentSlot->EmptySlot();
			// Remove Equipment from Component and Character
			CurrentDragOperation->SourceEquipmentSlot->OnUnequipItem.ExecuteIfBound(*CurrentDragOperation->ItemEntry);
			
			return true;
		}

		int32 ToSuperiorIndex = INDEX_NONE;
		int32 ToLowerIndex    = INDEX_NONE;

		//ResolvePair(ItemsArray, ToIndex, FromSize, MaxColumns, ToSuperiorIndex, ToLowerIndex);

		if (!ItemsArray.IsValidIndex(ToSuperiorIndex) || !ItemsArray.IsValidIndex(ToLowerIndex)) return false;
		
		FItemSlotData& TopSlot    = ItemsArray[ToSuperiorIndex];
		FItemSlotData& BottomSlot = ItemsArray[ToLowerIndex];

		if (TopSlot.bIsEmpty && BottomSlot.bIsEmpty)
		{
			// Crear item en grid
			TopSlot.Entry = *CurrentDragOperation->ItemEntry;
			TopSlot.Icon = CurrentDragOperation->ItemDefinition.Icon.Get();
			TopSlot.Size = ESlotSizeCategories::SuperiorSlotVertical;
			TopSlot.bIsEmpty = false;
			TopSlot.ItemDefinition = CurrentDragOperation->ItemDefinition;
			
			BottomSlot.Entry = *CurrentDragOperation->ItemEntry;
			BottomSlot.Icon = CurrentDragOperation->ItemDefinition.Icon.Get();
			BottomSlot.Size = ESlotSizeCategories::LowerSlotVertical;
			BottomSlot.bIsEmpty = false;
			BottomSlot.ItemDefinition = CurrentDragOperation->ItemDefinition;

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
	if (FromSize == ESlotSizeCategories::UniqueSlot)
	{
		if (ToSlot.bIsEmpty)
		{
			ToSlot = ItemsArray[FromIndex];
			ResetSlotData(ItemsArray[FromIndex]);
			return true;
		}
		if (ToSlot.Size == ESlotSizeCategories::UniqueSlot && ToIndex != FromIndex)
		{
			ItemsArray.Swap(ToIndex, FromIndex);
			return true;
		}
		return false;
	}
 
	// From is Superior or Lower
	if (ToSlot.Size == ESlotSizeCategories::UniqueSlot && !ToSlot.bIsEmpty) return false;
 
	const int32 FromSuperiorIndex = (FromSize == ESlotSizeCategories::SuperiorSlotVertical)
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
	int32 ToSuperiorIndex = ToIndex;
	int32 ToLowerIndex    = ToIndex + MaxColumns;
	//ResolvePair(ItemsArray, ToIndex, FromSize, MaxColumns, ToSuperiorIndex, ToLowerIndex);
 
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
 
	if (ToSuperiorSlot.Size == ESlotSizeCategories::SuperiorSlotVertical || ToSuperiorSlot.Size == ESlotSizeCategories::LowerSlotVertical)
	{
		ItemsArray.Swap(FromSuperiorIndex, ToSuperiorIndex);
		ItemsArray.Swap(FromLowerIndex,    ToLowerIndex);
		return true;
	}
 
	return false;
}
 

void UItemsPanelWidget::HandleDraggedItemLeaved(int32 DraggedIndex, int32 TargetIndex,EDragOverResult Result)
{
	if (ItemsArray.IsEmpty()) return;
	
	if (DraggedIndex == INDEX_NONE)
	{
		if (!CurrentDragOperation) return;

		FItemSlotData DraggedSlot;
		DraggedSlot.Size = CurrentDragOperation->SlotSize;

		int32 CleanTopIndex    = TargetIndex;
		int32 CleanBottomIndex = TargetIndex + MaxColumns;

		//ResolvePair(ItemsArray, TargetIndex, DraggedSlot.Size, MaxColumns, CleanTopIndex, CleanBottomIndex);

		if (CleanTopIndex == INDEX_NONE) return;

		UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(CleanTopIndex);
		UItemSlotWidget* BottomWidget = GetItemSlotbyIndex(CleanBottomIndex);

		if (IsValid(TopWidget))
		{
			TopWidget->DisableDragOverPreview();
		}
		if (IsValid(BottomWidget)) BottomWidget->DisableDragOverPreview();	

		return;
	}

	int32 CleanTopIndex = TargetIndex;
	int32 CleanBottomIndex = TargetIndex + MaxColumns;

	
	if (CleanTopIndex == INDEX_NONE) return;
 
	UItemSlotWidget* TopWidget    = GetItemSlotbyIndex(CleanTopIndex);
	UItemSlotWidget* BottomWidget = ItemsArray.IsValidIndex(CleanBottomIndex)
	                                  ? GetItemSlotbyIndex(CleanBottomIndex)
	                                  : nullptr;
 
	if (IsValid(TopWidget))
	{
		TopWidget->DisableDragOverPreview();
	}
	if (IsValid(BottomWidget)) BottomWidget->DisableDragOverPreview();
}
 

void UItemsPanelWidget::HandleDragCancelled(int32 FailedIndex, int32 FromIndex, ESlotSizeCategories DraggedSize)
{
	if (ItemsArray.IsEmpty()) return;
	
	// clean target
	int32 ToSuperior = FailedIndex, ToLower = FailedIndex + MaxColumns;
	if (ItemsArray.IsValidIndex(FailedIndex))
	{
		//ResolvePair(ItemsArray, FailedIndex, DraggedSize, MaxColumns, ToSuperior, ToLower);

		if (UItemSlotWidget* W = GetItemSlotbyIndex(ToSuperior)) W->DisableDragOverPreview();
		if (ItemsArray.IsValidIndex(ToLower))
			if (UItemSlotWidget* W = GetItemSlotbyIndex(ToLower)) W->DisableDragOverPreview();
	}

	// clean origin
	if (ItemsArray.IsValidIndex(FromIndex))
	{
		//int32 FromSuperior = INDEX_NONE, FromLower = INDEX_NONE;
		//ResolvePair(ItemsArray, FromIndex, DraggedSize, MaxColumns, FromSuperior, FromLower);

		if (UItemSlotWidget* W = GetItemSlotbyIndex(ToSuperior)) W->DisableDragOverPreview();
		if (ItemsArray.IsValidIndex(ToLower))
			if (UItemSlotWidget* W = GetItemSlotbyIndex(ToLower)) W->DisableDragOverPreview();
	}

	if (IsValid(CurrentDragOperation))
		CurrentDragOperation->ItemDraggedIconWidget->DisableDragOverResultIcon();
}

void UItemsPanelWidget::HandleSlotHovered(UBaseInventorySlot* BaseSlot)
{
    if (!IsValid(BaseSlot)) return;
    UItemSlotWidget* ItemSlotWidget = Cast<UItemSlotWidget>(BaseSlot);
    if (!IsValid(ItemSlotWidget) || !IsValid(ItemToolTipReference)) return;

    ESlotSizeCategories Size = ItemSlotWidget->GetCurrentSlotSize();
    int32 SlotGridIndex = ItemSlotWidget->GetGridIndex();

    if (ItemSlotWidget->IsEmpty())
    {
        ItemToolTipReference->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    // item definition
    FMasterItemDefinition ItemDefinition;
    if (ItemsArray.IsValidIndex(SlotGridIndex))
    {
        // Grid slot
        if (LastHoveredIndex == SlotGridIndex) return;
        if (Size == ESlotSizeCategories::SuperiorSlotVertical && LastHoveredIndex == SlotGridIndex + MaxColumns) return;
        if (Size == ESlotSizeCategories::LowerSlotVertical    && LastHoveredIndex == SlotGridIndex - MaxColumns) return;
        LastHoveredIndex = SlotGridIndex;

        ItemDefinition = ItemsArray[SlotGridIndex].ItemDefinition;
    }
    else
    {
        // Equipment slot
        ItemDefinition = BaseSlot->GetCurrentItemDefinition();
    }

	if (ItemDefinition.ItemTag.IsValid())
		ItemToolTipReference->SetData(ItemDefinition);

    // Tooltip position
    FGeometry SlotGeometry = ItemSlotWidget->GetCachedGeometry();
    if (Size == ESlotSizeCategories::SuperiorSlotVertical && ItemsArray.IsValidIndex(SlotGridIndex + MaxColumns))
    {
        UItemSlotWidget* LowerSlot = GetItemSlotbyIndex(SlotGridIndex + MaxColumns);
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
	float DPIScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	FVector2D CorrectedPos = ViewportPosition / DPIScale;
    ItemToolTipReference->SetPositionInViewport(CorrectedPos, false);
    ItemToolTipReference->SetVisibility(ESlateVisibility::Visible);
}

void UItemsPanelWidget::HandleSlotLeaved(UBaseInventorySlot* BaseSlot)
{
	if (!IsValid(BaseSlot)) return;
	UItemSlotWidget* ItemSlotWidget = Cast<UItemSlotWidget>(BaseSlot);
	if (!IsValid(ItemSlotWidget)) return;
	
	ESlotSizeCategories Size = ItemSlotWidget->GetCurrentSlotSize();
	int32 SlotGridIndex = ItemSlotWidget->GetGridIndex();

	// leaving pair
	int32 LeavingPairSuperior = INDEX_NONE;
	if      (Size == ESlotSizeCategories::SuperiorSlotVertical) LeavingPairSuperior = SlotGridIndex;
	else if (Size == ESlotSizeCategories::LowerSlotVertical)    LeavingPairSuperior = SlotGridIndex - MaxColumns;
	else                                   LeavingPairSuperior = SlotGridIndex; // ESlotSizeCategories::UniqueSlot
	
	int32 ActivePairSuperior = INDEX_NONE;
	if (LastHoveredIndex != INDEX_NONE)
	{
		if (ItemsArray.IsValidIndex(LastHoveredIndex))
		{
			ESlotSizeCategories ActiveSize = ItemsArray[LastHoveredIndex].Size;
			if      (ActiveSize == ESlotSizeCategories::SuperiorSlotVertical) ActivePairSuperior = LastHoveredIndex;
			else if (ActiveSize == ESlotSizeCategories::LowerSlotVertical)    ActivePairSuperior = LastHoveredIndex - MaxColumns;
			else                                         ActivePairSuperior = LastHoveredIndex;
		}
	}
	
	if (LeavingPairSuperior != ActivePairSuperior) return;
	
	LastHoveredIndex = INDEX_NONE;
	ItemToolTipReference->SetVisibility(ESlateVisibility::Collapsed);
}


void UItemsPanelWidget::HandleSlotClicked(int32 ClickedIndex)
{
	if (!ItemsArray.IsValidIndex(ClickedIndex)) return;
	if (ItemsArray[ClickedIndex].bIsEmpty) return;
	SelectCurrentIndexSlot(ClickedIndex);
}

