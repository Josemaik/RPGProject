// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Categories/BaseCategoryWidget.h"
#include "UI/Inventory/GridItemsPanelWidget.h"

void UBaseCategoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	for (UGridItemsPanelWidget* Panel : SubCategoryPanels)
	{
		Panel->OnSelectItemDelegate.BindUObject(this,&UBaseCategoryWidget::OnSubPanelSelected);
	}
}

void UBaseCategoryWidget::ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
	const FMasterItemDefinition& Definition)
{
	for (UGridItemsPanelWidget* Panel : SubCategoryPanels)
	{
		if (!IsValid(Panel)) return;
		if (Definition.SubcategoryTag.MatchesTagExact(Panel->GetSubCategoryTag()))
		{
			Panel->AddItemSlot(Entry, Definition);
		}
	}
}

void UBaseCategoryWidget::RemoveEntry(const FRPGInventoryEntry& Entry)
{
	for (UGridItemsPanelWidget* Panel : SubCategoryPanels)
	{
		if (!IsValid(Panel)) return;
		Panel->RemoveItem(Entry);
	}
}

const FRPGInventoryEntry* UBaseCategoryWidget::GetSelectedItem()
{
	for (UGridItemsPanelWidget* Panel : SubCategoryPanels)
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
	for (UGridItemsPanelWidget* Panel : SubCategoryPanels)
	{
		Panel->SortItemsQuicly();
	}
}

void UBaseCategoryWidget::SortPanels(EItemSortType SortType)
{
	for (UGridItemsPanelWidget* Panel : SubCategoryPanels)
	{
		Panel->SortItemsBy(SortType);
	}
}

void UBaseCategoryWidget::OnSubPanelSelected(FGameplayTag NewSelectedCategory)
{
	if (SubCategoryPanels.IsEmpty()) return;

	const bool bSameCategory = NewSelectedCategory.MatchesTagExact(LastSelectedSubCategory);

	for (UGridItemsPanelWidget* Panel : SubCategoryPanels)
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
