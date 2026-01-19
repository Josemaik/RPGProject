// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RPGSystemsWidget.h"

#include "AbilitySystem/RPGGameplayTags.h"
#include "Components/EditableText.h"
#include "Components/HorizontalBox.h"
#include "Components/Spacer.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Interfaces/InventoryInterface.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/CategoryButton.h"
#include "UI/Inventory/ItemRowWidget.h"
#include "UI/WidgetController/InventoryWidgetController.h"
#include "UI/WidgetController/WidgetController.h"

void URPGSystemsWidget::SetWidgetController(UWidgetController* InWidgetController)
{
	WidgetController = InWidgetController;
	CurrentCategorySelected = RPGGameplayTags::InventoryItems::EquipmentTag;
	CacheEssentialVars();
	BindInventoryItemDelegates();
}

void URPGSystemsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SearchBar->OnTextChanged.AddDynamic(this, &ThisClass::OnSearchBarTextChanged);
	
	CategoriesContainer->ClearChildren();
	for (const FCategoryButtonData& Data : Categories)
	{
		UCategoryButton* Button =
			CreateWidget<UCategoryButton>(this, CategoryButtonClass);

		Button->Init(Data.CategoryTag, Data.DisplayName, Data.Icon);

		Button->OnCategorySelected.AddLambda([this](FGameplayTag CategoryTag)
		{
			HandleCategorySelected(CategoryTag);
		});

		CategoriesContainer->AddChildToHorizontalBox(Button);

		USpacer* Spacer = NewObject<USpacer>(this);
		Spacer->SetSize(FVector2d(8.f,1.f));

		CategoriesContainer->AddChildToHorizontalBox(Spacer);
	}
}

void URPGSystemsWidget::FinishDestroy()
{
	Super::FinishDestroy();
	if (!IsValid(InventoryWidgetController))
	{
		return;
	}
	
	InventoryWidgetController->InventoryEntryDelegate.RemoveAll(this);
}

void URPGSystemsWidget::CacheEssentialVars()
{
	if (!IsValid(WidgetController))
	{
		return;
	}

	InventoryWidgetController = Cast<UInventoryWidgetController>(WidgetController);
	
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(InventoryWidgetController->GetOwningActor());

	if (!IsValid(OwningInventory))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::Printf(TEXT("No valid Inventory")));
	}

	const TArray<FRPGInventoryEntry>& Entries = OwningInventory->InventoryList.GetEntries();
	if (Entries.IsEmpty()) return;
	for (const FRPGInventoryEntry& Entry : Entries)
	{
		HandleInventoryItemReceived(Entry);
	}
}

void URPGSystemsWidget::BindInventoryItemDelegates()
{
	InventoryWidgetController->InventoryEntryDelegate.AddUObject(this,&URPGSystemsWidget::HandleInventoryItemReceived);
	InventoryWidgetController->OnInventoryItemRemoved.AddUObject(this,&URPGSystemsWidget::HandleInventoryItemRemoved);
}

void URPGSystemsWidget::ClearEntries()
{
	TArray<int64> keys;
	ActiveItemSlotWidgets.GetKeys(keys);
	
	for (auto& key : keys)
	{
		HandleInventoryItemRemoved(key);
	}

	ActiveItemSlotWidgets.Empty();
}

void URPGSystemsWidget::HandleCategorySelected(FGameplayTag CategorySelected)
{
	if (CurrentCategorySelected.MatchesTagExact(CategorySelected)) return;
	
	//New Category
	CurrentCategorySelected = CategorySelected;
	CurrentCategoryIndex = 0;
	ItemsPanel->ClearChildren();

	for (const auto& Pair : ActiveItemSlotWidgets)
	{
		if (Pair.Value->ItemEntry.ItemTag.MatchesTag(CategorySelected) && IsValid(Pair.Value))
		{
			AddToItemsGrid(Pair.Value);
		}
	}
	
}

void URPGSystemsWidget::AddToItemsGrid(TObjectPtr<UItemRowWidget> ItemSlot)
{
	if (!IsValid(CurrentItemSlotWidget)) return;
	
	const int32 Index = CurrentCategoryIndex;
		
	const int32 Row = Index / MaxColumns;
	const int32 Column = Index % MaxColumns;
		
	UUniformGridSlot* GridSlot = ItemsPanel->AddChildToUniformGrid(ItemSlot);
		
	if (IsValid(GridSlot))
	{
		GridSlot->SetRow(Row);
		GridSlot->SetColumn(Column);
	}
		
	ItemSlot->SetGridSlot(Index,GridSlot);

	CurrentCategoryIndex++;
}

void URPGSystemsWidget::HandleInventoryItemReceived(const FRPGInventoryEntry& Entry)
{
	if (!IsValid(OwningInventory)) return;
	if (UItemRowWidget** FoundWidgetPtr = ActiveItemSlotWidgets.Find(Entry.ItemID))
	{
		if (UItemRowWidget* FoundWidget = *FoundWidgetPtr)
		{
			FoundWidget->SetQuantityText(Entry.Quantity);
			return;
		}
	}

	FMasterItemDefinition ItemDefinition = OwningInventory->GetItemDefinitionByTag(Entry.ItemTag);
	
	CurrentItemSlotWidget = Cast<UItemRowWidget>(CreateWidget(this,ItemSlotWidget));
	
	if (!IsValid(CurrentItemSlotWidget))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::Printf(TEXT("Item Widget is null")));
		return;
	}
	CurrentItemSlotWidget->SetInventoryEntry(Entry, ItemDefinition.Icon);
	CurrentItemSlotWidget->SetActionText(Entry.ItemTag);
	CurrentItemSlotWidget->SetItemNameText(Entry.ItemName);
	CurrentItemSlotWidget->SetQuantityText(Entry.Quantity);

	//bind delegates
	CurrentItemSlotWidget->OnUseButtomClickedDelegate.BindLambda(
		[this](const FRPGInventoryEntry& Entry)
		{
			OwningInventory->UseItem(Entry,1);
			if (Entry.Quantity == 0)
			{
				//El Slot queda vacío, entonces hay que reordenar los slots
				//recorrer active item row widgets, ver cuales tienen mayor index y desplazarlos a una casilla anterior
			}
		});
	CurrentItemSlotWidget->OnItemRowClickedDelegate.BindLambda(
		[this](const FRPGInventoryEntry& Entry)
		{
			HandleItemRowClicked(Entry);
		});
	CurrentItemSlotWidget->OnItemDroppedEventDelegate.BindUObject(this,&ThisClass::HandleItemDropped);

	ActiveItemSlotWidgets.Add(Entry.ItemID,CurrentItemSlotWidget);
	
	//Check category to add to Grid
	if (Entry.ItemTag.MatchesTag(CurrentCategorySelected))
	{
		AddToItemsGrid(CurrentItemSlotWidget);
	}
}

void URPGSystemsWidget::HandleItemRowClicked(const FRPGInventoryEntry& Entry)
{
	if (/*!IsValid(ItemDescriptionText) ||*/ !IsValid(OwningInventory))
	{
		return;
	}
	
	const FMasterItemDefinition& ItemDefiniton = OwningInventory->GetItemDefinitionByTag(Entry.ItemTag);
	//ItemDescriptionText->SetText(ItemDefiniton.ItemDescription);
}

void URPGSystemsWidget::HandleItemDropped(const FRPGInventoryEntry& Entry)
{
	if (!IsValid(OwningInventory)) return;
	
	OwningInventory->DropItem(Entry,Entry.Quantity);
	
	HandleInventoryItemRemoved(Entry.ItemID);
}

void URPGSystemsWidget::HandleInventoryItemRemoved(const int64 ItemID)
{
	if (UItemRowWidget* ItemRow = ActiveItemSlotWidgets.FindChecked(ItemID))
	{
		ItemRow->OnUseButtomClickedDelegate.Unbind();
		ItemRow->OnItemRowClickedDelegate.Unbind();
		ItemRow->OnItemDroppedEventDelegate.Unbind();
		ItemRow->RemoveFromParent();
		ActiveItemSlotWidgets.Remove(ItemID);
	}
}

void URPGSystemsWidget::OnSearchBarTextChanged(const FText& InText)
{
	if (!IsValid(OwningInventory))
	{
		return;
	}
	
	CurrentCategoryIndex = 0;
	ItemsPanel->ClearChildren();

	TArray<FRPGInventoryEntry> SearchedEntries = OwningInventory->GetEntriesByString(InText.ToString());
	
	if (SearchedEntries.IsEmpty()) return;
	
	for (const FRPGInventoryEntry& Entry : SearchedEntries)
	{
		if (Entry.ItemTag.MatchesTag(CurrentCategorySelected) && ActiveItemSlotWidgets.Contains(Entry.ItemID))
		{
			 if (UItemRowWidget* FoundWidget = *ActiveItemSlotWidgets.Find(Entry.ItemID))
			 {
			 	AddToItemsGrid(FoundWidget);
			 }
		}
	}
}