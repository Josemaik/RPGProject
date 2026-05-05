// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/InventoryWidget.h"

#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/HorizontalBox.h"
#include "Components/Spacer.h"
#include "Components/WidgetSwitcher.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemCategoryButton.h"
#include "UI/Inventory/ItemsDropToWorldWidget.h"
#include "UI/Inventory/ItemSlotWidget.h"
#include "UI/Inventory/ItemsPanelWidget.h"
#include "UI/Inventory/ItemToolTip.h"
#include "UI/Inventory/SortPanelWidget.h"
#include "UI/Inventory/Categories/BaseCategoryWidget.h"
#include "UI/Inventory/Categories/EquipmentCategoryWidget.h"
#include "UI/Inventory/Equipment/EquipmentSlot.h"
#include "UI/WidgetController/InventoryWidgetController.h"

void UInventoryWidget::SetWidgetController(UInventoryWidgetController* InWidgetController)
{
	InventoryWidgetController = InWidgetController;
	
	CurrentCategorySelected = RPGInventoryTags::ItemsCategory::Equipment;
	//ItemsContainer->CurrentCategoryTag = CurrentCategorySelected;
	//Add category widgets to switcher and map
	for (const FCategoryButtonData& Data : Categories)
	{
		if (TSubclassOf<UBaseCategoryWidget>* WidgetClass = CategoryWidgetClasses.Find(Data.CategoryTag))
		{
			UBaseCategoryWidget* CategoryWidget = CreateWidget<UBaseCategoryWidget>(this, *WidgetClass);
			CategorySwitcher->AddChild(CategoryWidget);
			CategoryWidgets.Add(Data.CategoryTag, CategoryWidget);
			if (Data.CategoryTag == RPGInventoryTags::ItemsCategory::Equipment)
			{
				UEquipmentCategoryWidget* EquipmentCatWidget = Cast<UEquipmentCategoryWidget>(CategoryWidget);
				if (!IsValid(EquipmentCatWidget)) return;
				EquipmentCatWidget->OnEquipmentDropped.BindUObject(this, &UInventoryWidget::OnEquipmentDropped);
				CategorySwitcher->SetActiveWidget(CategoryWidget);
			}
		}
	}
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

		if (Data.CategoryTag.MatchesTagExact(RPGInventoryTags::ItemsCategory::Equipment))
		{
			Button->Select();
		}

		CategoriesContainer->AddChildToHorizontalBox(Button);

		USpacer* Spacer = NewObject<USpacer>(this);
		Spacer->SetSize(FVector2d(8.f,1.f));

		CategoriesContainer->AddChildToHorizontalBox(Spacer);

		CategoryButtonsMap.Add(Data.CategoryTag, Button);
	}
}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	//SearchBar->OnTextChanged.AddDynamic(this, &ThisClass::OnSearchBarTextChanged);
	
	//funcion bind inventoryequipmentdelegates
	ItemToolTipReference = CreateWidget<UItemToolTip>(GetOwningPlayer(), TooltipWidgetClass);
	ItemToolTipReference->AddToViewport(999); 
	ItemToolTipReference->SetVisibility(ESlateVisibility::Collapsed);
	
	SilverSword->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	SteelWeapon->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	Bolls->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	RangedWeapon->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	ConsumableSlot0->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	ConsumableSlot1->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	ConsumableSlot2->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	ConsumableSlot3->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	ChestSlot->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	BootsSlot->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	GauntletSlot->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);
	TrousersSLot->OnEquipItem.BindUObject(this, &ThisClass::OnEquipItem);

	EquipmentSlots.Add(SilverSword);
	EquipmentSlots.Add(SteelWeapon);
	EquipmentSlots.Add(Bolls);
	EquipmentSlots.Add(RangedWeapon);
	EquipmentSlots.Add(ConsumableSlot0);
	EquipmentSlots.Add(ConsumableSlot1);
	EquipmentSlots.Add(ConsumableSlot2);
	EquipmentSlots.Add(ConsumableSlot3);
	EquipmentSlots.Add(ChestSlot);
	EquipmentSlots.Add(BootsSlot);
	EquipmentSlots.Add(GauntletSlot);
	EquipmentSlots.Add(TrousersSLot);
	
	InitEquipmentWidget(SilverSword);
	InitEquipmentWidget(Bolls);
	InitEquipmentWidget(SteelWeapon);
	InitEquipmentWidget(RangedWeapon);
	InitEquipmentWidget(ConsumableSlot0);
	InitEquipmentWidget(ConsumableSlot1);
	InitEquipmentWidget(ConsumableSlot2);
	InitEquipmentWidget(ConsumableSlot3);
	InitEquipmentWidget(ChestSlot);
	InitEquipmentWidget(BootsSlot);
	InitEquipmentWidget(GauntletSlot);
	InitEquipmentWidget(TrousersSLot);
	
	ItemsDropToWorldWidget->OnItemDroppedPanelDelegate.BindUObject(this, &ThisClass::HandleItemDropped);

	QuickSortButton->OnClicked.AddDynamic(this, &ThisClass::OnQuickSortButtonClicked);
	SortButton->OnClicked.AddDynamic(this, &ThisClass::OnSortButtonClicked);

	SortPanelWidget->OnCloseButtonClicked.BindUObject(this, &ThisClass::OnSortPanelCloseButtonClicked);
	SortPanelWidget->OnOptionChanged.BindUObject(this, &ThisClass::OnSortPanelOptionChanged);
}

void UInventoryWidget::HandleUnequipItem(const FRPGInventoryEntry& Entry)
{
	InventoryWidgetController->UnEquipItem(Entry.ItemTag);
}

void UInventoryWidget::InitEquipmentWidget(UEquipmentSlot* EquipmentSlot)
{
	EquipmentSlot->SetTooltipReference(ItemToolTipReference); // el mismo del panel
	EquipmentSlot->OnUnequipItem.BindUObject(this, &UInventoryWidget::HandleUnequipItem);
	EquipmentSlot->OnEquipmentDropped.BindUObject(this,&UInventoryWidget::OnEquipmentDropped);
	EquipmentSlot->OnSwapToPanelDelegate.BindLambda([this](const FRPGInventoryEntry& OldEntry)
	{
		InventoryWidgetController->AddEquippedItem(OldEntry.ItemTag,OldEntry.ItemID);
		//HandleInventoryItemReceived(OldEntry);
	});
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

void UInventoryWidget::OnEquipmentDropped(FGameplayTag ItemTag,uint64 ExistingID)
{
	InventoryWidgetController->AddEquippedItem(ItemTag,ExistingID);
	InventoryWidgetController->UnEquipItem(ItemTag);
}

void UInventoryWidget::BindInventoryItemDelegates()
{
	InventoryWidgetController->InventoryEntryDelegate.AddUObject(this,&UInventoryWidget::HandleInventoryItemReceived);
	InventoryWidgetController->OnInventoryItemRemoved.AddUObject(this,&UInventoryWidget::HandleInventoryItemRemoved);
	InventoryWidgetController->OnSortItemsRequested.AddUObject(this, &UInventoryWidget::SortItems);
	InventoryWidgetController->OnEquipKeyPressed.AddUObject(this,&UInventoryWidget::OnEquipKeyPressed);
	InventoryWidgetController->OnDropKeyPressed.AddUObject(this,&UInventoryWidget::OnDropKeyPressed);
	//ItemsContainer->OnEquipmentDropped.BindUObject(this,&UInventoryWidget::OnEquipmentDropped);
}

void UInventoryWidget::HandleCategorySelected(FGameplayTag CategorySelected)
{
	if (CurrentCategorySelected.MatchesTagExact(CategorySelected)) return;

	if (UItemCategoryButton* LastButton = CategoryButtonsMap.FindRef(CurrentCategorySelected))
	{
		LastButton->DeSelect();
	}

	if (UItemCategoryButton* NewButton = CategoryButtonsMap.FindRef(CategorySelected))
	{
		NewButton->Select();
		LastCategorySelected = NewButton; // opcional si quieres mantenerlo
	}
	
	//New Category
	CurrentCategorySelected = CategorySelected;

	if (UBaseCategoryWidget** Widget = CategoryWidgets.Find(CategorySelected))
	{
		CategorySwitcher->SetActiveWidget(*Widget);
	}
}

void UInventoryWidget::SortItems(bool Quickly)
{
	//if (!IsValid(ItemsContainer)) return;
	UBaseCategoryWidget* ActiveCategory = Cast<UBaseCategoryWidget>(CategorySwitcher->GetActiveWidget());
	if (!IsValid(ActiveCategory)) return;
	if (Quickly)
	{
		ActiveCategory->QuickSortPanels();
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
	UBaseCategoryWidget* ActiveCategory = Cast<UBaseCategoryWidget>(CategorySwitcher->GetActiveWidget());
	if (!IsValid(ActiveCategory)) return;
	ActiveCategory->SortPanels(ItemSort);
	//ItemsContainer->SortItemsBy(ItemSort);
}

void UInventoryWidget::HandleInventoryItemReceived(const FRPGInventoryEntry& Entry)
{
	const FMasterItemDefinition& ItemDefinition = InventoryWidgetController->GetInventoryItemDefinition(Entry.ItemTag);

	UBaseCategoryWidget** FoundWidget = CategoryWidgets.Find(ItemDefinition.CategoryTag);
	if (FoundWidget == nullptr) return;
	
	UBaseCategoryWidget* CurrentCategoryWidget = *FoundWidget;
	if (!IsValid(CurrentCategoryWidget)) return;

	CurrentCategoryWidget->ReceiveInventoryEntry(Entry, ItemDefinition);
}

void UInventoryWidget::HandleItemDropped(const FRPGInventoryEntry& Entry) const
{
	InventoryWidgetController->DropItemToWorld(Entry);
}

void UInventoryWidget::HandleInventoryItemRemoved(const FRPGInventoryEntry& Entry)
{
	const FMasterItemDefinition& ItemDefinition = InventoryWidgetController->GetInventoryItemDefinition(Entry.ItemTag);

	UBaseCategoryWidget** FoundWidget = CategoryWidgets.Find(ItemDefinition.CategoryTag);
	if (FoundWidget == nullptr) return;

	UBaseCategoryWidget* CurrentCategoryWidget = *FoundWidget;
	if (!IsValid(CurrentCategoryWidget)) return;

	CurrentCategoryWidget->RemoveEntry(Entry);
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
	//ItemsContainer->RemoveItem(Entry.ItemID);
}

void UInventoryWidget::OnEquipKeyPressed()
{
	//if (CurrentCategorySelected != RPGInventoryTags::ItemsCategory::Equipment) return;

	UBaseCategoryWidget* EquipmentCategoryWidget = *CategoryWidgets.Find(CurrentCategorySelected);
	if (!IsValid(EquipmentCategoryWidget)) return;

	const FRPGInventoryEntry SelectedItem = *EquipmentCategoryWidget->GetSelectedItem();
	// const FRPGInventoryEntry& SelectedItem = ItemsContainer->GetSelectedItem();
	if (SelectedItem.ItemID == INDEX_NONE) return;
	
	const FMasterItemDefinition& ItemDefinition = InventoryWidgetController->GetInventoryItemDefinition(SelectedItem.ItemTag);

	for (UEquipmentSlot* EquipmentSlot: EquipmentSlots)
	{
		if (EquipmentSlot->GetSlotTag().MatchesTagExact(ItemDefinition.SlotTag))
		{
			if (EquipmentSlot->GetSlotTag().MatchesTagExact(RPGInventoryTags::EquipmentSlot::Consumable))
			{
				if (!EquipmentSlot->IsEmpty()) continue;
				EquipmentSlot->EquipItem(SelectedItem,ItemDefinition);
			}
			else
			{
				EquipmentSlot->EquipItem(SelectedItem,ItemDefinition);
			}
			
			break;
		}	
	}
	//InventoryWidgetController->EquipItem(SelectedItem);
}

void UInventoryWidget::OnDropKeyPressed()
{
	UBaseCategoryWidget* CurrentCategoryWidget = *CategoryWidgets.Find(CurrentCategorySelected);
	if (!IsValid(CurrentCategoryWidget)) return;

	const FRPGInventoryEntry& SelectedItem = *CurrentCategoryWidget->GetSelectedItem();
	//const FRPGInventoryEntry& SelectedItem = ItemsContainer->GetSelectedItem();
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
