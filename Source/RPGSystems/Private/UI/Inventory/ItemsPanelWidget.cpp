// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemsPanelWidget.h"

#include <rapidjson/reader.h>

#include "GameDelegates.h"
#include "IDetailTreeNode.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Engine/AssetManager.h"
#include "UI/Inventory/ItemSlotDroppedDragDrop.h"
#include "UI/Inventory/ItemSlotWidget.h"

struct FStreamableManager;

namespace FGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Category_Equipment, "Item.Equipment");
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

void UItemsPanelWidget::AddItemSlot(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& ItemDefinition)
{
	TArray<FItemSlotData>* ItemsArrayPtr = CategoryItemsMap.Find(GetItemCategory(Entry.ItemTag));
	if (!ItemsArrayPtr) return;

	TArray<FItemSlotData>& ItemsArray = *ItemsArrayPtr;

	int32 FreeIndex = INDEX_NONE;

	for (int32 i = 0; i < ItemsArray.Num(); i++)
	{
		if (ItemsArray[i].bIsEmpty)
		{
			FreeIndex = i;
			break;
		}
	}

	if (FreeIndex == INDEX_NONE) return;

	FSlateBrush Brush;
	Brush.SetResourceObject(ItemDefinition.Icon.Get());

	ItemsArray[FreeIndex].Entry = Entry;
	ItemsArray[FreeIndex].Icon = ItemDefinition.Icon;
	ItemsArray[FreeIndex].bIsEmpty = false;
	ItemsArray[FreeIndex].Size = UniqueSlot;
	
	if (Entry.ItemTag.MatchesTag(FGameplayTags::Category_Equipment) && ItemDefinition.SlotsSize == 2)
	{   //Slot Size == 2
		// FBox2d UVRegionLower(FVector2d(0.0, 0.5), FVector2d(1.0, 1.0));
		// Brush.SetUVRegion(UVRegionLower);
		ItemsArray[FreeIndex].Size = SuperiorSlotVertical;
		ItemsArray[FreeIndex + MaxColumns].Entry = Entry;
		ItemsArray[FreeIndex + MaxColumns].Icon = ItemDefinition.Icon;
		ItemsArray[FreeIndex + MaxColumns].bIsEmpty = false;
		ItemsArray[FreeIndex + MaxColumns].Size = LowerSlotVertical;
		//ItemsArray[FreeIndex + MaxColumns].IconBrush = Brush;
					
		// FBox2d UVRegionSuperior(FVector2d(0.0, 0.0), FVector2d(1.0, 0.5));
		// Brush.SetUVRegion(UVRegionSuperior);
		
		
		//ItemsArray[FreeIndex].IconBrush = Brush;
	}

	// 🔥 reconstruir UI SOLO si estás en esa categoría
	if (GetItemCategory(Entry.ItemTag) == CurrentCategoryTag)
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

void UItemsPanelWidget::OnIconLoaded(UItemSlotWidget* Widget, FItemSlotData SlotData)
{
	if (!IsValid(Widget)) return;

	UTexture2D* LoadedTexture = SlotData.Icon.Get();
	if (!LoadedTexture) return;

	FSlateBrush Brush;
	Brush.SetResourceObject(LoadedTexture);

	Widget->Init(SlotData.Entry, SlotData.Icon, Brush, ESlotSizeCategories::UniqueSlot);
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
		FItemSlotData& SlotData = ItemsArray[i];
		
		UItemSlotWidget* NewWidget = CreateWidget<UItemSlotWidget>(GetOwningPlayer(), ItemSlotWidgetClass);
		if (!IsValid(NewWidget)) continue;

		if (!SlotData.bIsEmpty)
		{   //Added Item
			if (SlotData.Icon.IsNull())
			{
				continue;
			}

			if (SlotData.Icon.IsValid())
			{
				FSlateBrush Brush;
				Brush.SetResourceObject(SlotData.Icon.Get());
				
				if (SlotData.Size == LowerSlotVertical)
				{
					FBox2d UVRegionLower(FVector2d(0.0, 0.5), FVector2d(1.0, 1.0));
					Brush.SetUVRegion(UVRegionLower);

					NewWidget->Init(SlotData.Entry, SlotData.Icon, Brush, LowerSlotVertical);
					NewWidget->OnItemDroppedPanelDelegate.BindUObject(this,&UItemsPanelWidget::HandleItemDropped);
					NewWidget->OnDragEnteredDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemEntered);
					NewWidget->OnDragLeavedDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemLeaved);
					AddItemToGrid(NewWidget, i);
					continue;
				}
				
				if (SlotData.Size == SuperiorSlotVertical)
				{
					FBox2d UVRegionSuperior(FVector2d(0.0, 0.0), FVector2d(1.0, 0.5));
					Brush.SetUVRegion(UVRegionSuperior);
					
					NewWidget->Init(SlotData.Entry, SlotData.Icon, Brush, SuperiorSlotVertical);
				}
				else
				{
					NewWidget->Init(SlotData.Entry, SlotData.Icon, Brush, UniqueSlot);
				}

				NewWidget->OnItemDroppedPanelDelegate.BindUObject(this,&UItemsPanelWidget::HandleItemDropped);
				NewWidget->OnDragEnteredDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemEntered);
				NewWidget->OnDragLeavedDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemLeaved);
				AddItemToGrid(NewWidget, i);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Icon texture Not Loaded")));
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    
				TWeakObjectPtr<UItemSlotWidget> WeakWidget = NewWidget;

				Streamable.RequestAsyncLoad(
					SlotData.Icon.ToSoftObjectPath(),
					FStreamableDelegate::CreateUObject(this, &UItemsPanelWidget::OnIconLoaded, WeakWidget.Get(), SlotData)
				);
			}
		}
		else
		{
			// Bind delegates
			NewWidget->EmptySlot();
			NewWidget->OnItemDroppedPanelDelegate.BindUObject(this,&UItemsPanelWidget::HandleItemDropped);
			NewWidget->OnDragEnteredDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemEntered);
			NewWidget->OnDragLeavedDelegate.BindUObject(this,&UItemsPanelWidget::HandleDraggedItemLeaved);
			AddItemToGrid(NewWidget, i);
		}
	}
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

	int32 FromIndex = DroppedSlot->GetGridIndex();
	int32 ToIndex = NewSlot->GetGridIndex();

	ESlotSizeCategories FromSize = DroppedSlot->GetCurrentSlotSize();
	ESlotSizeCategories ToSize   = NewSlot->GetCurrentSlotSize();
	
	if ((FromIndex == ToIndex) || (FromSize == SuperiorSlotVertical && ToIndex == FromIndex + MaxColumns)
		|| (FromSize == LowerSlotVertical && ToIndex == FromIndex - MaxColumns)) return;
	
	
	TArray<FItemSlotData>* ItemsArrayPtr = CategoryItemsMap.Find(CurrentCategoryTag);
	if (!ItemsArrayPtr) return;

	TArray<FItemSlotData>& ItemsArray = *ItemsArrayPtr;
	
	

	if (!ItemsArray.IsValidIndex(FromIndex) || !ItemsArray.IsValidIndex(ToIndex)) return;

	

	//Save Data origin
	FItemSlotData FromMain = ItemsArray[FromIndex];
	FItemSlotData FromSecond;
	
	if (FromSize == UniqueSlot)
	{
		ItemsArray[FromIndex] = FromMain;
	}

	//Save Slot two if origin slot size == 2
	if (FromSize == SuperiorSlotVertical && ItemsArray.IsValidIndex(FromIndex + MaxColumns))
	{
		FromSecond = ItemsArray[FromIndex + MaxColumns];
	}
	else if (FromSize == LowerSlotVertical && ItemsArray.IsValidIndex(FromIndex - MaxColumns))
	{
		FromSecond = ItemsArray[FromIndex - MaxColumns];
	}

	//Clean origin
	ItemsArray[FromIndex] = FItemSlotData();

	if (FromSize == SuperiorSlotVertical && ItemsArray.IsValidIndex(FromIndex + MaxColumns))
	{
		ItemsArray[FromIndex + MaxColumns] = FItemSlotData();
	}
	else if (FromSize == LowerSlotVertical && ItemsArray.IsValidIndex(FromIndex - MaxColumns))
	{
		ItemsArray[FromIndex - MaxColumns] = FItemSlotData();
	}

	ItemsArray[ToIndex] = FItemSlotData();

	if (ToSize == SuperiorSlotVertical && ItemsArray.IsValidIndex(ToIndex + MaxColumns))
	{
		ItemsArray[ToIndex + MaxColumns] = FItemSlotData();
	}
	else if (ToSize == LowerSlotVertical && ItemsArray.IsValidIndex(ToIndex - MaxColumns))
	{
		ItemsArray[ToIndex - MaxColumns] = FItemSlotData();
	}

	ItemsArray[ToIndex] = FromMain;

	if (FromSize == SuperiorSlotVertical && ItemsArray.IsValidIndex(ToIndex + MaxColumns))
	{
		ItemsArray[ToIndex + MaxColumns] = FromSecond;
	}
	else if (FromSize == LowerSlotVertical && ItemsArray.IsValidIndex(ToIndex - MaxColumns))
	{
		ItemsArray[ToIndex - MaxColumns] = FromSecond;
	}
	
	// 🔥 rebuild
	ResetCategory(CurrentCategoryTag);
}

void UItemsPanelWidget::HandleDraggedItemEntered(int32 NewIndex)
{
	if (!ItemsPanel) return;

	if (UWidget* Child = ItemsPanel->GetChildAt(NewIndex + MaxColumns))
	{
		if (UItemSlotWidget* ItemSlotWidget = Cast<UItemSlotWidget>(Child))
		{
			ItemSlotWidget->OutlineSlot(ESlotSizeCategories::LowerSlotVertical);
		}
	}
}

void UItemsPanelWidget::HandleDraggedItemLeaved(int32 NewIndex)
{
	if (!ItemsPanel) return;

	TArray<FItemSlotData>* ItemsArrayPtr = CategoryItemsMap.Find(CurrentCategoryTag);
	if (!ItemsArrayPtr) return;

	if (UWidget* Child = ItemsPanel->GetChildAt(NewIndex + MaxColumns))
	{
		if (UItemSlotWidget* ItemSlotWidget = Cast<UItemSlotWidget>(Child))
		{
			ItemSlotWidget->RemoveOutLineSlot(false);
		}
	}
}
