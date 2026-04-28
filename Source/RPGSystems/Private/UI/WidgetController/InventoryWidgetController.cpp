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
		OwningInventory->InventoryList.DirtyItemDelegate.AddLambda(
			[this](const FRPGInventoryEntry& DirtyItem)
			{
				InventoryEntryDelegate.Broadcast(DirtyItem);
			});

		OwningInventory->InventoryList.InventoryItemRemovedDelegate.AddLambda(
			[this](const FRPGInventoryEntry& ItemRemoved)
			{
				OnInventoryItemRemoved.Broadcast(ItemRemoved);
			});
		OwningInventory->OnWeightChanged.AddLambda(
			[this](float Weight)
			{
				OnInventoryWeightChanged.Broadcast(Weight);
			});
	}
}

void UInventoryWidgetController::BroadCastInitialValues() const
{
	if (IsValid(OwningInventory))
	{
		float TotalWeight = 0;
		for (const FRPGInventoryEntry& Entry : OwningInventory->GetInventoryEntries())
		{
			const FMasterItemDefinition& EntryDefinition = GetInventoryItemDefinition(Entry.ItemTag);
			InventoryEntryDelegate.Broadcast(Entry);
			TotalWeight += EntryDefinition.Weight;
		}
		OnInventoryWeightChanged.Broadcast(TotalWeight);
	}
}

void UInventoryWidgetController::EquipItem(const FRPGInventoryEntry& Entry) const
{
	if (!IsValid(OwningInventory)) return;
	
	OwningInventory->UseItem(Entry,1);
	
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("numentries: %d"),OwningInventory->InventoryList.GetEntries().Num()));
}

void UInventoryWidgetController::AddEquipItem(FGameplayTag ItemTag,uint64 ExistingID) const
{
	if (!IsValid(OwningInventory)) return;
	
	OwningInventory->AddItem(ItemTag,ExistingID);

	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("numentries: %d"),OwningInventory->InventoryList.GetEntries().Num()));
}

void UInventoryWidgetController::DropItemToWorld(const FRPGInventoryEntry& Entry) const
{
	if (!IsValid(OwningInventory)) return;

	OwningInventory->DropItem(Entry,Entry.Quantity);
}

void UInventoryWidgetController::RequestSortItems(bool Quick) const
{
	OnSortItemsRequested.Broadcast(Quick);
}

float UInventoryWidgetController::GetMaxInventoryWeight() const
{
	if (!IsValid(OwningInventory)) return 0;

	return OwningInventory->GetMaxInventoryWeight();
}

const FMasterItemDefinition UInventoryWidgetController::GetInventoryItemDefinition(FGameplayTag ItemTag) const
{
	return OwningInventory->GetItemDefinitionByTag(ItemTag);
}
