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
}

void URPGSystemsWidget::HandleInventoyItemReceived(const FRPGInventoryEntry& Entry)
{
	if (UItemRowWidget** FoundPtr = ActiveItemRowWidgets.Find(Entry.ItemID))
	{
		if (UItemRowWidget* FoundWidget = *FoundPtr)
		{
			if (Entry.Quantity < 0)
			{
				return;
			}
			
			if (Entry.Quantity == 0)
			{
				FoundWidget->OnUseButtomClickedDelegate.Unbind();
				FoundWidget->RemoveFromParent();
				ActiveItemRowWidgets.Remove(Entry.ItemID);
				return;
			}
			
			FoundWidget->SetQuantityText(Entry.Quantity);
			return;
		}
	}
	
	CurrentItemRowWidget = Cast<UItemRowWidget>(CreateWidget(this,ItemRowWidgetClass));
	
	if (!IsValid(CurrentItemRowWidget))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,FString::Printf(TEXT("Item Widget is null")));
		return;
	}
	CurrentItemRowWidget->SetInventoryEntry(Entry);
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
}
