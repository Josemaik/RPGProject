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
	
	InventoryWidgetController->InventoryItemDelegate.RemoveAll(this);
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
}

void URPGSystemsWidget::HandleInventoyItemReceived(const FMasterItemDefinition& Item)
{
	if (UItemRowWidget** FoundPtr = ActiveItemRowWidgets.Find(Item.ItemTag))
	{
		if (UItemRowWidget* FoundWidget = *FoundPtr)
		{
			if (Item.ItemQuantity < 0)
			{
				return;
			}
			
			if (Item.ItemQuantity == 0)
			{
				FoundWidget->OnUseButtomClickedDelegate.Unbind();
				FoundWidget->RemoveFromParent();
				ActiveItemRowWidgets.Remove(Item.ItemTag);
				return;
			}
			
			FoundWidget->SetQuantityText(Item.ItemQuantity);
			return;
		}
	}
	
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
	
	ActiveItemRowWidgets.Add(Item.ItemTag,CurrentItemRowWidget);
	
	CurrentItemRowWidget->OnUseButtomClickedDelegate.BindLambda(
		[this](const FGameplayTag& Tag)
		{
			OwningInventory->UseItem(Tag,1);
		});
}
