// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Categories/BaseCategoryWidget.h"
#include "UI/Inventory/ItemsPanelWidget.h"

void UBaseCategoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (UItemsPanelWidget* Panel : SubCategoryPanels)
	{
		Panel->OnSelectItemDelegate.BindUObject(this,&UBaseCategoryWidget::OnSubPanelSelected);
	}
}

void UBaseCategoryWidget::ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
	const FMasterItemDefinition& Definition)
{
	for (UItemsPanelWidget* Panel : SubCategoryPanels)
	{
		if (!IsValid(Panel)) return;
		if (Definition.SubcategoryTag.MatchesTagExact(Panel->GetSubCategoryTag()))
		{
			Panel->AddItemSlot(Entry, Definition);
		}
	}
}

void UBaseCategoryWidget::RemoveEntry(int64 ItemID)
{
	for (UItemsPanelWidget* Panel : SubCategoryPanels)
	{
		if (!IsValid(Panel)) return;
		Panel->RemoveItem(ItemID);
	}
}

const FRPGInventoryEntry* UBaseCategoryWidget::GetSelectedItem()
{
	for (UItemsPanelWidget* Panel : SubCategoryPanels)
	{
		if (LastSelectedSubCategory.MatchesTagExact(Panel->GetSubCategoryTag()))
		{
			return Panel->GetSelectedItem();
		}
	}
	return nullptr;
}

void UBaseCategoryWidget::QuickSortPanels()
{
	for (UItemsPanelWidget* Panel : SubCategoryPanels)
	{
		Panel->SortItemsQuicly();
	}
}

void UBaseCategoryWidget::SortPanels(EItemSortType SortType)
{
	for (UItemsPanelWidget* Panel : SubCategoryPanels)
	{
		Panel->SortItemsBy(SortType);
	}
}

void UBaseCategoryWidget::OnSubPanelSelected(FGameplayTag NewSelectedCategory)
{
	if (SubCategoryPanels.IsEmpty()) return;

	const bool bSameCategory = NewSelectedCategory.MatchesTagExact(LastSelectedSubCategory);

	for (UItemsPanelWidget* Panel : SubCategoryPanels)
	{
		const FGameplayTag PanelTag = Panel->GetSubCategoryTag();

		if (PanelTag.MatchesTagExact(NewSelectedCategory))
		{
			Panel->ActivateCurrentSelectedSlot();
		}
		else if (!bSameCategory && PanelTag.MatchesTagExact(LastSelectedSubCategory))
		{
			Panel->DeactivateCurrentSelectedSlot();
		}
	}

	LastSelectedSubCategory = NewSelectedCategory;
}
