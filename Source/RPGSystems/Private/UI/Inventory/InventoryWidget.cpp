// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryWidget.h"

#include "AbilitySystem/RPGGameplayTags.h"
#include "Components/EditableText.h"
#include "Components/HorizontalBox.h"
#include "Components/Spacer.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Interfaces/InventoryInterface.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemCategoryButton.h"
#include "UI/Inventory/ItemSlotWidget.h"
#include "UI/Inventory/Equipment/EquipmentSlot.h"
#include "UI/WidgetController/InventoryWidgetController.h"
#include "UI/WidgetController/WidgetController.h"

void UInventoryWidget::SetWidgetController(UInventoryWidgetController* InWidgetController)
{
	InventoryWidgetController = InWidgetController;
	CurrentCategorySelected = RPGGameplayTags::InventoryItems::EquipmentTag;
	CacheEssentialVars();
	BindInventoryItemDelegates();
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SearchBar->OnTextChanged.AddDynamic(this, &ThisClass::OnSearchBarTextChanged);
	
	CategoriesContainer->ClearChildren();
	for (const FCategoryButtonData& Data : Categories)
	{
		UItemCategoryButton* Button =
			CreateWidget<UItemCategoryButton>(this, CategoryButtonClass);

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

	LeftHandEquipment->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	RightHandEquipment->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
}

bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryWidget::FinishDestroy()
{
	Super::FinishDestroy();
	if (!IsValid(InventoryWidgetController))
	{
		return;
	}
	
	InventoryWidgetController->InventoryEntryDelegate.RemoveAll(this);
}

void UInventoryWidget::CacheEssentialVars()
{
	// if (!IsValid(WidgetController))
	// {
	// 	return;
	// }
	//
	// InventoryWidgetController = Cast<UInventoryWidgetController>(WidgetController);
	if (!IsValid(InventoryWidgetController)) return;
	
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(InventoryWidgetController->GetOwningActor());

	if (!IsValid(OwningInventory))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::Printf(TEXT("No valid Inventory")));
	}

	// const TArray<FRPGInventoryEntry>& Entries = OwningInventory->InventoryList.GetEntries();
	// if (Entries.IsEmpty()) return;
	// for (const FRPGInventoryEntry& Entry : Entries)
	// {
	// 	HandleInventoryItemReceived(Entry);
	// }
}

void UInventoryWidget::BindInventoryItemDelegates()
{
	InventoryWidgetController->InventoryEntryDelegate.AddUObject(this,&UInventoryWidget::HandleInventoryItemReceived);
	InventoryWidgetController->OnInventoryItemRemoved.AddUObject(this,&UInventoryWidget::HandleInventoryItemRemoved);
}

void UInventoryWidget::ClearEntries()
{
	//ActiveItems.Empty();
}

void UInventoryWidget::HandleCategorySelected(FGameplayTag CategorySelected)
{
	if (CurrentCategorySelected.MatchesTagExact(CategorySelected)) return;
	if (!IsValid(OwningInventory)) return;
	
	//New Category
	CurrentCategorySelected = CategorySelected;
	CurrentCategoryIndex = 0;
	ItemsPanel->ClearChildren();

	for (const FRPGInventoryEntry& ItemEntry : OwningInventory->InventoryList.GetEntries())
	{
		if (ItemEntry.ItemTag.MatchesTag(CategorySelected))
		{
			UItemSlotWidget* NewItemSlot = NewActiveItem(ItemEntry);
			if (IsValid(NewItemSlot))
			{
				AddToItemsGrid(NewItemSlot);
			}
		}
	}
	
}

void UInventoryWidget::AddToItemsGrid(TObjectPtr<UItemSlotWidget> InSlotWidget)
{
	if (!IsValid(InSlotWidget)) return;
	
	const int32 Index = CurrentCategoryIndex;
		
	const int32 Row = Index / MaxColumns;
	const int32 Column = Index % MaxColumns;
		
	UUniformGridSlot* GridSlot = ItemsPanel->AddChildToUniformGrid(InSlotWidget);
		
	if (IsValid(GridSlot))
	{
		GridSlot->SetRow(Row);
		GridSlot->SetColumn(Column);
	}
		
	InSlotWidget->SetGridSlot(Index,GridSlot);

	CurrentCategoryIndex++;
}

UItemSlotWidget* UInventoryWidget::NewActiveItem(const FRPGInventoryEntry& Entry)
{
	if (!IsValid(OwningInventory)) return nullptr;

	FMasterItemDefinition ItemDefinition = OwningInventory->GetItemDefinitionByTag(Entry.ItemTag);
	
	UItemSlotWidget* CurrentItemSlotWidget = Cast<UItemSlotWidget>(CreateWidget(this,ItemSlotWidgetClass));
	
	if (!IsValid(CurrentItemSlotWidget))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::Printf(TEXT("Item Widget is null")));
		return nullptr;
	}
	
	CurrentItemSlotWidget->Init(Entry, ItemDefinition.Icon,InventoryWidgetController);

	CurrentItemSlotWidget->OnItemRowClickedDelegate.BindLambda(
		[this](const FRPGInventoryEntry& Entry)
		{
			HandleItemRowClicked(Entry);
		});

	return CurrentItemSlotWidget;
}

void UInventoryWidget::HandleInventoryItemReceived(const FRPGInventoryEntry& Entry)
{
	// if (UItemSlotWidget** FoundWidgetPtr = ActiveItems.Find(Entry))
	// {
	// 	if (UItemSlotWidget* FoundWidget = *FoundWidgetPtr)
	// 	{
	// 		FoundWidget->SetQuantityText(Entry.Quantity);
	// 		return;
	// 	}
	// }
	
	//bind delegates
	// CurrentItemSlotWidget->OnUseButtomClickedDelegate.BindLambda(
	// 	[this](const FRPGInventoryEntry& Entry)
	// 	{
			//OwningInventory->UseItem(Entry,1);
			//if (Entry.Quantity == 0)
			//{
				//El Slot queda vacío, entonces hay que reordenar los slots
				//recorrer active item row widgets, ver cuales tienen mayor index y desplazarlos a una casilla anterior
			//}
	//	});
	UItemSlotWidget* NewSlotWidget = NewActiveItem(Entry);
	if (!IsValid(NewSlotWidget)) return;
	
	//Check category to add to Grid
	if (Entry.ItemTag.MatchesTag(CurrentCategorySelected))
	{
		AddToItemsGrid(NewSlotWidget);
	}
}


void UInventoryWidget::HandleItemRowClicked(const FRPGInventoryEntry& Entry)
{
	if (/*!IsValid(ItemDescriptionText) ||*/ !IsValid(OwningInventory))
	{
		return;
	}
	
	const FMasterItemDefinition& ItemDefiniton = OwningInventory->GetItemDefinitionByTag(Entry.ItemTag);
	//ItemDescriptionText->SetText(ItemDefiniton.ItemDescription);
}

void UInventoryWidget::HandleItemDropped(const FRPGInventoryEntry& Entry)
{
	if (!IsValid(OwningInventory)) return;

	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Equipment dropped in World")));
	
	OwningInventory->DropItem(Entry,Entry.Quantity);
}

void UInventoryWidget::HandleInventoryItemRemoved(const int64 ItemID)
{
	// if (!IsValid(OwningInventory)) return;
	//
	// FRPGInventoryEntry* FindedEntry = nullptr;
	// for (FRPGInventoryEntry& Entry : OwningInventory->InventoryList.GetEntries())
	// {
	// 	if (Entry.ItemID == ItemID)
	// 	{
	// 		FindedEntry = &Entry;
	// 		break;
	// 	}
	// }
	//
	// if (!FindedEntry) return;
	//
	// ActiveItems.Remove(*FindedEntry);
}

void UInventoryWidget::OnSearchBarTextChanged(const FText& InText)
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
		if (Entry.ItemTag.MatchesTag(CurrentCategorySelected))
		{
			UItemSlotWidget* NewItemSlot = NewActiveItem(Entry);
			if (IsValid(NewItemSlot))
			{
				AddToItemsGrid(NewItemSlot);
			}
		}
	}
}

void UInventoryWidget::OnEquipItem(const FRPGInventoryEntry& Entry)
{
	if (!IsValid(OwningInventory)) return;
	
	OwningInventory->UseItem(Entry,1);
}
