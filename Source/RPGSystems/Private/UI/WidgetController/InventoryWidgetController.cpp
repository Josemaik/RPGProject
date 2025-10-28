// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/InventoryWidgetController.h"

#include "Interfaces/InventoryInterface.h"
#include "InventorySection/InventoryComponent.h"

void UInventoryWidgetController::SetOwningActor(AActor* InOwner)
{
	OwningActor = InOwner;
}

void UInventoryWidgetController::BindCallbacksToDependencies()
{
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(OwningActor);

	if (IsValid(OwningInventory))
	{
		OwningInventory->InventoryPackakdgedDelegate.AddLambda(
			[this](const FPackedInventory& InventoryContent)
			{
				UpdateInventory(InventoryContent);
			});
	}
}

void UInventoryWidgetController::BroadCastInitialValues()
{
	if (!IsValid(OwningInventory))
	{
		return;
	}

	BroadcastInventoryContent();
}

void UInventoryWidgetController::UpdateInventory(const FPackedInventory& InInventory)
{
	if (IsValid(OwningInventory))
	{
		OwningInventory->ReconstructInventory(InInventory);

		BroadcastInventoryContent();
	}
}

void UInventoryWidgetController::BroadcastInventoryContent()
{
	if (IsValid(OwningInventory))
	{
		const TMap<FGameplayTag, int32>& LocalInventoryMap = OwningInventory->GetInventoryMap();

		ScrollBoxResetDelegate.Broadcast();
		
		for (const auto& Pair : LocalInventoryMap)
		{
			FMasterItemDefinition Item = OwningInventory->GetItemDefinitionByTag(Pair.Key);
			Item.ItemQuantity = Pair.Value;
			InventoryItemDelegate.Broadcast(Item);
		}

		InventoryBroadcastCompleteDelegate.Broadcast();
	}
}
