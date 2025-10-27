// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySection/InventoryComponent.h"

#include "Net/UnrealNetwork.h"

bool FPackedInventory::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	SafeNetSerializeTArray_WithNetSerialize<100>(Ar,ItemTags,Map);
	SafeNetSerializeTArray_Default<100>(Ar,ItemQuantities);

	bOutSuccess = true;
	return true;
}

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, CadchedInventory);
}

void UInventoryComponent::AddItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	if (!Owner->HasAuthority())
	{
		ServerAddItem(ItemTag,NumItems);
		return;
	}

	if (InventortyTagMap.Contains(ItemTag))
	{
		InventortyTagMap[ItemTag] += NumItems;
	}
	else
	{
		InventortyTagMap.Emplace(ItemTag,NumItems);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		FString::Printf(TEXT("Server Items Added to Inventory %s, qty: %d"), *ItemTag.ToString(), NumItems));

	PackageInventory(CadchedInventory);
}

void UInventoryComponent::ServerAddItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems)
{
	AddItem(ItemTag,NumItems);
}

void UInventoryComponent::PackageInventory(FPackedInventory& OutInventory)
{
	OutInventory.ItemTags.Empty();
	OutInventory.ItemQuantities.Empty();

	for (const auto& Pair : InventortyTagMap)
	{
		if (Pair.Value > 0)
		{
			OutInventory.ItemTags.Add(Pair.Key);
			OutInventory.ItemQuantities.Add(Pair.Value);
		}
	}
}

void UInventoryComponent::ReconstructInventory(const FPackedInventory& InInventory)
{
	InventortyTagMap.Empty();
	
	for (int32 i = 0; i < InInventory.ItemTags.Num(); ++i)
	{
		InventortyTagMap.Emplace(InInventory.ItemTags[i], InInventory.ItemQuantities[i]);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue,
		FString::Printf(TEXT("Tag Added: %s // QuantityAdded_ %d"), *InInventory.ItemTags[i].ToString(),
			InInventory.ItemQuantities[i]));
	}
}

void UInventoryComponent::OnRep_CachedInventory()
{
	ReconstructInventory(CadchedInventory);
}

