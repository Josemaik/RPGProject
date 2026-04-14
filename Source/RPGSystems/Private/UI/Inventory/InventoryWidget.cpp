// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryWidget.h"

#include "ShaderPrintParameters.h"
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
#include "UI/Inventory/ItemsPanelWidget.h"
#include "UI/Inventory/Equipment/EquipmentSlot.h"
#include "UI/WidgetController/InventoryWidgetController.h"
#include "UI/WidgetController/WidgetController.h"

void UInventoryWidget::SetWidgetController(UInventoryWidgetController* InWidgetController)
{
	InventoryWidgetController = InWidgetController;
	CurrentCategorySelected = RPGGameplayTags::InventoryItems::EquipmentTag;
	ItemsContainer->CurrentCategoryTag = CurrentCategorySelected;
	CacheEssentialVars();
	BindInventoryItemDelegates();

	//Init categories
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

		ItemsContainer->AddEmptySlots(Data.CategoryTag);
	}
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SearchBar->OnTextChanged.AddDynamic(this, &ThisClass::OnSearchBarTextChanged);

	//bind warpbox->childs->childs(castear a UEquipmentSlot) y delegate
	//funcion bind inventoryequipmentdelegates
	SilverSword->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	SteelWeapon->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	Bolls->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	RangedWeapon->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
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
	if (!IsValid(InventoryWidgetController)) return;
	
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(InventoryWidgetController->GetOwningActor());

	if (!IsValid(OwningInventory))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::Printf(TEXT("No valid Inventory")));
	}
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
	ItemsContainer->ResetCategory(CurrentCategorySelected);
	
	// if (!IsValid(ItemsContainer)) return;
	// ItemsContainer->ClearPanel();
	//
	// for (const FRPGInventoryEntry& ItemEntry : OwningInventory->InventoryList.GetEntries())
	// {
	// 	if (ItemEntry.ItemTag.MatchesTag(CategorySelected))
	// 	{
	// 		UItemSlotWidget* NewItemSlot = NewActiveItem(ItemEntry);
	// 		if (IsValid(NewItemSlot))
	// 		{
	// 			AddToItemsGrid(NewItemSlot);
	// 		}
	// 	}
	// }
	
}

void UInventoryWidget::AddToItemsGrid(TObjectPtr<UItemSlotWidget> InSlotWidget)
{
	if (!IsValid(InSlotWidget) || !IsValid(ItemsContainer)) return;

	//ItemsContainer->AddItem(InSlotWidget);
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

	CurrentItemSlotWidget->Init(Entry, ItemDefinition.Icon);

	CurrentItemSlotWidget->OnItemRowClickedDelegate.BindLambda(
		[this](const FRPGInventoryEntry& Entry)
		{
			HandleItemRowClicked(Entry);
		});

	CurrentItemSlotWidget->OnItemDroppedEventDelegate.BindLambda(
		[this](const FRPGInventoryEntry& Entry)
		{
			HandleItemDropped(Entry);
		});
	

	return CurrentItemSlotWidget;
}

void UInventoryWidget::BindItemSlotDelegates(UItemSlotWidget* CurrentItemSlotWidget)
{
	if (IsValid(CurrentItemSlotWidget))
	{
		CurrentItemSlotWidget->OnItemRowClickedDelegate.BindLambda(
			[this](const FRPGInventoryEntry& Entry)
			{
				HandleItemRowClicked(Entry);
			});

		CurrentItemSlotWidget->OnItemDroppedEventDelegate.BindLambda(
			[this](const FRPGInventoryEntry& Entry)
			{
				HandleItemDropped(Entry);
			});	
	}
}

void UInventoryWidget::AddItemToGrid(const FRPGInventoryEntry& Entry, const FMasterItemDefinition& ItemDefinition)
{
	if (!IsValid(ItemsContainer))
	{
		return;
	}
	
	if (ItemDefinition.SlotsSize == 2)
	{
		UItemSlotWidget* SuperiorSlot = ItemsContainer->AddItemSlot(Entry,ItemDefinition);
		BindItemSlotDelegates(SuperiorSlot);
		
		if (!IsValid(SuperiorSlot)) return;
		UItemSlotWidget* LowerSlot = ItemsContainer->AddItemSlot(Entry,ItemDefinition,SuperiorSlot->GetGridIndex());
		
		if (!IsValid(LowerSlot)) return;
		BindItemSlotDelegates(LowerSlot);
		
		return;
	}
	
	UItemSlotWidget* CurrentItemSlotWidget = ItemsContainer->AddItemSlot(Entry,ItemDefinition);
	if (!IsValid(CurrentItemSlotWidget)) return;
	BindItemSlotDelegates(CurrentItemSlotWidget);
}

void UInventoryWidget::HandleInventoryItemReceived(const FRPGInventoryEntry& Entry)
{
	// if (!Entry.ItemTag.MatchesTag(CurrentCategorySelected))
	// {
	// 	return;
	// }
	
	if (!IsValid(ItemsContainer)) return;
	if (ItemsContainer->ContainsItemSlot(Entry.ItemID))
	{
		ItemsContainer->UpdateItemSlot(Entry);
		return;
	}

	if (!IsValid(OwningInventory)) return;

	const FMasterItemDefinition& ItemDefinition = OwningInventory->GetItemDefinitionByTag(Entry.ItemTag);

	AddItemToGrid(Entry, ItemDefinition);
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

	//Unbind delegates
}

void UInventoryWidget::HandleInventoryItemRemoved(const int64 ItemID)
{
	ItemsContainer->RemoveItem(ItemID);

	//Unbind delegates
}

void UInventoryWidget::OnSearchBarTextChanged(const FText& InText)
{
	if (!IsValid(OwningInventory))
	{
		return;
	}
	
	ItemsContainer->ResetCategory(CurrentCategorySelected);

	if (!IsValid(ItemsContainer)) return;
	ItemsContainer->ClearPanel();

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
