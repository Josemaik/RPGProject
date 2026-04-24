// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryWidget.h"

#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/HorizontalBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemCategoryButton.h"
#include "UI/Inventory/ItemsDropToWorldWidget.h"
#include "UI/Inventory/ItemSlotWidget.h"
#include "UI/Inventory/ItemsPanelWidget.h"
#include "UI/Inventory/SortPanelWidget.h"
#include "UI/Inventory/Equipment/EquipmentSlot.h"
#include "UI/WidgetController/InventoryWidgetController.h"

void UInventoryWidget::SetWidgetController(UInventoryWidgetController* InWidgetController)
{
	InventoryWidgetController = InWidgetController;
	
	CurrentCategorySelected = RPGInventoryTags::ItemsCategory::Equipment;
	ItemsContainer->CurrentCategoryTag = CurrentCategorySelected;
	//CacheEssentialVars();
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
	
	//funcion bind inventoryequipmentdelegates
	SilverSword->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	SteelWeapon->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	Bolls->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	RangedWeapon->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);

	ItemsDropToWorldWidget->OnItemDroppedPanelDelegate.BindUObject(this, &ThisClass::HandleItemDropped);

	QuickSortButton->OnClicked.AddDynamic(this, &ThisClass::OnQuickSortButtonClicked);
	SortButton->OnClicked.AddDynamic(this, &ThisClass::OnSortButtonClicked);

	SortPanelWidget->OnCloseButtonClicked.BindUObject(this, &ThisClass::OnSortPanelCloseButtonClicked);
	SortPanelWidget->OnOptionChanged.BindUObject(this, &ThisClass::OnSortPanelOptionChanged);
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

void UInventoryWidget::BindInventoryItemDelegates()
{
	InventoryWidgetController->InventoryEntryDelegate.AddUObject(this,&UInventoryWidget::HandleInventoryItemReceived);
	InventoryWidgetController->OnInventoryItemRemoved.AddUObject(this,&UInventoryWidget::HandleInventoryItemRemoved);
	InventoryWidgetController->OnSortItemsRequested.AddUObject(this, &UInventoryWidget::SortItems);
	InventoryWidgetController->OnEquipKeyPressed.AddUObject(this,&UInventoryWidget::OnEquipKeyPressed);
	InventoryWidgetController->OnDropKeyPressed.AddUObject(this,&UInventoryWidget::OnDropKeyPressed);
}

void UInventoryWidget::HandleCategorySelected(FGameplayTag CategorySelected)
{
	if (CurrentCategorySelected.MatchesTagExact(CategorySelected)) return;
	
	//New Category
	CurrentCategorySelected = CategorySelected;
	ItemsContainer->ResetCategory(CurrentCategorySelected);
	
	FString TagString = CurrentCategorySelected.GetTagName().ToString();
	FString RightPart;
	TagString.Split(TEXT("."), nullptr, &RightPart, ESearchCase::IgnoreCase, ESearchDir::FromEnd);

	CategoryText->SetText(FText::FromString(RightPart));
}

UItemSlotWidget* UInventoryWidget::NewActiveItem(const FRPGInventoryEntry& Entry)
{
	FMasterItemDefinition ItemDefinition = InventoryWidgetController->GetInventoryItemDefinition(Entry.ItemTag);
	
	UItemSlotWidget* CurrentItemSlotWidget = Cast<UItemSlotWidget>(CreateWidget(this,ItemSlotWidgetClass));
	
	if (!IsValid(CurrentItemSlotWidget))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::Printf(TEXT("Item Widget is null")));
		return nullptr;
	}

	CurrentItemSlotWidget->Init(Entry, ItemDefinition.Icon);

	return CurrentItemSlotWidget;
}

void UInventoryWidget::AddItemToGrid(const FRPGInventoryEntry& Entry, const FMasterItemDefinition& ItemDefinition)
{
	if (!IsValid(ItemsContainer))
	{
		return;
	}

	ItemsContainer->AddItemSlot(Entry,ItemDefinition);
}

void UInventoryWidget::SortItems(bool Quickly)
{
	if (!IsValid(ItemsContainer)) return;
	
	if (Quickly)
	{
		ItemsContainer->SortItemsQuicly();
	}
	else
	{
		SetSortPanelVisibility();
	}
}

void UInventoryWidget::OnQuickSortButtonClicked()
{
	SortItems(true);
}

void UInventoryWidget::OnSortButtonClicked()
{
	SortItems(false);
}

void UInventoryWidget::OnSortPanelCloseButtonClicked()
{
	HideSortPanel();
}

void UInventoryWidget::OnSortPanelOptionChanged(EItemSortType ItemSort)
{
	HideSortPanel();
	ItemsContainer->SortItemsBy(ItemSort);
}

void UInventoryWidget::HandleInventoryItemReceived(const FRPGInventoryEntry& Entry)
{
	if (!IsValid(ItemsContainer)) return;
	if (ItemsContainer->FindItemIndex(Entry.ItemID,Entry.ItemTag) != INDEX_NONE)
	{
		ItemsContainer->UpdateItemSlot(Entry);
		return;
	}

	const FMasterItemDefinition& ItemDefinition = InventoryWidgetController->GetInventoryItemDefinition(Entry.ItemTag);
	AddItemToGrid(Entry, ItemDefinition);
}

void UInventoryWidget::HandleItemDropped(const FRPGInventoryEntry& Entry) const
{

	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Equipment dropped in World")));
	
	InventoryWidgetController->DropItemToWorld(Entry);
}

void UInventoryWidget::HandleInventoryItemRemoved(const FRPGInventoryEntry& Entry) const
{
	ItemsContainer->RemoveItem(Entry.ItemID);

	//Unbind delegates
	
}

void UInventoryWidget::OnSearchBarTextChanged(const FText& InText)
{
	// if (!IsValid(OwningInventory))
	// {
	// 	return;
	// }
	//
	// ItemsContainer->ResetCategory(CurrentCategorySelected);
	//
	// if (!IsValid(ItemsContainer)) return;
	// ItemsContainer->ClearPanel();
	//
	// TArray<FRPGInventoryEntry> SearchedEntries = OwningInventory->GetEntriesByString(InText.ToString());
	//
	// if (SearchedEntries.IsEmpty()) return;
	//
	// for (const FRPGInventoryEntry& Entry : SearchedEntries)
	// {
	// 	if (Entry.ItemTag.MatchesTag(CurrentCategorySelected))
	// 	{
	// 		UItemSlotWidget* NewItemSlot = NewActiveItem(Entry);
	// 		if (IsValid(NewItemSlot))
	// 		{
	// 			//AddToItemsGrid(NewItemSlot);
	// 		}
	// 	}
	// }
}

void UInventoryWidget::OnEquipItem(const FRPGInventoryEntry& Entry)
{
	if (!IsValid(InventoryWidgetController)) return;
	
	InventoryWidgetController->EquipItem(Entry);
}

void UInventoryWidget::OnEquipKeyPressed()
{
	const FRPGInventoryEntry& SelectedItem = ItemsContainer->GetSelectedItem();
	if (SelectedItem.ItemID == INDEX_NONE) return;
	
	InventoryWidgetController->EquipItem(SelectedItem);
	const FMasterItemDefinition& ItemDefinition = InventoryWidgetController->GetInventoryItemDefinition(SelectedItem.ItemTag);
	SilverSword->EquipItemSlot(SelectedItem,ItemDefinition.Icon.Get());
}

void UInventoryWidget::OnDropKeyPressed()
{
	const FRPGInventoryEntry& SelectedItem = ItemsContainer->GetSelectedItem();
	if (SelectedItem.ItemID == INDEX_NONE) return;
	
	InventoryWidgetController->DropItemToWorld(SelectedItem);
}

void UInventoryWidget::SetSortPanelVisibility()
{
	bShowSortPanelToggle = !bShowSortPanelToggle;
	if (bShowSortPanelToggle)
	{
		SortPanelWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		HideSortPanel();
	}
}

void UInventoryWidget::HideSortPanel()
{
	bShowSortPanelToggle = false;
	SortPanelWidget->SetVisibility(ESlateVisibility::Collapsed);
}
