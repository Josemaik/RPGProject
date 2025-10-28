// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RPGSystemsWidget.h"

#include "Components/ScrollBox.h"
#include "Interfaces/InventoryInterface.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemRowWidget.h"
#include "UI/WidgetController/InventoryWidgetController.h"
#include "UI/WidgetController/WidgetController.h"

void URPGSystemsWidget::SetWidgetController(UWidgetController* InWidgetController)
{
	WidgetController = InWidgetController;
	//OnWidgetControllerSet();
	CacheEssentialVars();
	BindInventoryItemDelegates();
}

void URPGSystemsWidget::FinishDestroy()
{
	Super::FinishDestroy();
	if (!IsValid(InventoryWidgetController))
	{
		return;
	}
	
	InventoryWidgetController->InventoryItemDelegate.RemoveAll(this);
	InventoryWidgetController->InventoryBroadcastCompleteDelegate.RemoveAll(this);
	InventoryWidgetController->ScrollBoxResetDelegate.RemoveAll(this);
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
	InventoryWidgetController->InventoryItemDelegate.AddUObject(this,&URPGSystemsWidget::HandleInventoyItemReceived);
	InventoryWidgetController->InventoryBroadcastCompleteDelegate.AddUObject(this,&URPGSystemsWidget::OnInventoryItemBroadcastComplete);
	InventoryWidgetController->ScrollBoxResetDelegate.AddUObject(this,&URPGSystemsWidget::OnScrollBoxReset);
}

void URPGSystemsWidget::HandleInventoyItemReceived(const FMasterItemDefinition& Item)
{
	CurrentItemRowWidget = Cast<UItemRowWidget>(CreateWidget(this,ItemRowWidgetClass));
	
	if (!IsValid(CurrentItemRowWidget))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::Printf(TEXT("Item Widget is null")));
		return;
	}
	CurrentItemRowWidget->SetItemDefinition(Item);
	CurrentItemRowWidget->SetActionText(Item.ItemTag);
	CurrentItemRowWidget->SetItemNameText(Item.ItemName);
	CurrentItemRowWidget->SetQuantityText(Item.ItemQuantity);

	InventoryContent->AddChild(CurrentItemRowWidget);
	
	ActiveItemRowWidgets.Add(CurrentItemRowWidget);
}

void URPGSystemsWidget::OnInventoryItemBroadcastComplete()
{
	for (const auto& ItemRow: ActiveItemRowWidgets)
	{
		if (!IsValid(ItemRow))
		{
			continue;
		}
		
		ItemRow->OnUseButtomClickedDelegate.BindLambda([this,ItemRow]()
		{
			OwningInventory->UseItem(ItemRow->ItemDefinition.ItemTag,1.f);
		});
	}
}

void URPGSystemsWidget::OnScrollBoxReset()
{
	InventoryContent->ClearChildren();
	for (const auto& ItemRow: ActiveItemRowWidgets)
	{
		ItemRow->OnUseButtomClickedDelegate.Unbind();
	}
	ActiveItemRowWidgets.Empty();
}
