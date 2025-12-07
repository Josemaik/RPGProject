// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RPGSystemsWidget.h"

#include "ContentBrowserDataSource.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Interfaces/InventoryInterface.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemRowWidget.h"
#include "UI/WidgetController/InventoryWidgetController.h"
#include "UI/WidgetController/WidgetController.h"

void URPGSystemsWidget::SetWidgetController(UWidgetController* InWidgetController)
{
	WidgetController = InWidgetController;
	CacheEssentialVars();
	BindInventoryItemDelegates();
}

void URPGSystemsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SearchBar->OnTextChanged.AddDynamic(this, &ThisClass::OnSearchBarTextChanged);
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
}

void URPGSystemsWidget::BindInventoryItemDelegates()
{
	InventoryWidgetController->InventoryEntryDelegate.AddUObject(this,&URPGSystemsWidget::HandleInventoyItemReceived);
	InventoryWidgetController->OnInventoryItemRemoved.AddUObject(this,&URPGSystemsWidget::HandleInventoryItemRemoved);
}

void URPGSystemsWidget::ClearEntries()
{
	TArray<int64> keys;
	ActiveItemRowWidgets.GetKeys(keys);
	
	for (auto& key : keys)
	{
		HandleInventoryItemRemoved(key);
	}

	ActiveItemRowWidgets.Empty();
}

void URPGSystemsWidget::HandleInventoyItemReceived(const FRPGInventoryEntry& Entry)
{
	if (!IsValid(OwningInventory)) return;
	if (UItemRowWidget** FoundWidgetPtr = ActiveItemRowWidgets.Find(Entry.ItemID))
	{
		if (UItemRowWidget* FoundWidget = *FoundWidgetPtr)
		{
			FoundWidget->SetQuantityText(Entry.Quantity);
			return;
		}
	}

	FMasterItemDefinition ItemDefinition = OwningInventory->GetItemDefinitionByTag(Entry.ItemTag);
	
	CurrentItemRowWidget = Cast<UItemRowWidget>(CreateWidget(this,ItemRowWidgetClass));
	
	if (!IsValid(CurrentItemRowWidget))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::Printf(TEXT("Item Widget is null")));
		return;
	}
	CurrentItemRowWidget->SetInventoryEntry(Entry, ItemDefinition.Icon);
	CurrentItemRowWidget->SetActionText(Entry.ItemTag);
	CurrentItemRowWidget->SetItemNameText(Entry.ItemName);
	CurrentItemRowWidget->SetQuantityText(Entry.Quantity);

	InventoryContent->AddChild(CurrentItemRowWidget);
	
	ActiveItemRowWidgets.Add(Entry.ItemID,CurrentItemRowWidget);
	
	CurrentItemRowWidget->OnUseButtomClickedDelegate.BindLambda(
		[this](const FRPGInventoryEntry& Entry)
		{
			OwningInventory->UseItem(Entry,1);
		});
	CurrentItemRowWidget->OnItemRowClickedDelegate.BindLambda(
		[this](const FRPGInventoryEntry& Entry)
		{
			HandleItemRowClicked(Entry);
		});
	CurrentItemRowWidget->OnItemDroppedEventDelegate.BindUObject(this,&ThisClass::HandleItemDropped);
}

void URPGSystemsWidget::HandleItemRowClicked(const FRPGInventoryEntry& Entry)
{
	if (!IsValid(ItemDescriptionText) || !IsValid(OwningInventory))
	{
		return;
	}
	
	const FMasterItemDefinition& ItemDefiniton = OwningInventory->GetItemDefinitionByTag(Entry.ItemTag);
	ItemDescriptionText->SetText(ItemDefiniton.ItemDescription);
}

void URPGSystemsWidget::HandleItemDropped(const FRPGInventoryEntry& Entry)
{
	if (!IsValid(OwningInventory)) return;
	
	OwningInventory->DropItem(Entry,Entry.Quantity);
	//HandleInventoryItemRemoved(Entry.ItemID);
}

void URPGSystemsWidget::HandleInventoryItemRemoved(const int64 ItemID)
{
	if (UItemRowWidget* ItemRow = ActiveItemRowWidgets.FindChecked(ItemID))
	{
		ItemRow->OnUseButtomClickedDelegate.Unbind();
		ItemRow->OnItemRowClickedDelegate.Unbind();
		ItemRow->OnItemDroppedEventDelegate.Unbind();
		ItemRow->RemoveFromParent();
		ActiveItemRowWidgets.Remove(ItemID);
	}
}

void URPGSystemsWidget::OnSearchBarTextChanged(const FText& InText)
{
	if (!IsValid(OwningInventory))
	{
		return;
	}

	ClearEntries();

	TArray<FRPGInventoryEntry> SearchedEntries = OwningInventory->GetEntriesByString(InText.ToString());

	for (const FRPGInventoryEntry& Entry : SearchedEntries)
	{
		HandleInventoyItemReceived(Entry);
	}
}