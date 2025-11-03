// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentManagerComponent.h"

#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentInstance.h"
#include "Net/UnrealNetwork.h"


UEquipmentInstance* FRPGEquipmentList::AddEntry(const TSubclassOf<UEquipmentDefinition>& InEquipmentDefinition)
{
	check(InEquipmentDefinition);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const UEquipmentDefinition* EquipmentCTO = GetDefault<UEquipmentDefinition>(InEquipmentDefinition);
	TSubclassOf<UEquipmentInstance> InstanceType = EquipmentCTO->InstanceType;

	if (!IsValid(InstanceType))
	{
		InstanceType = UEquipmentInstance::StaticClass();
	}

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGEquipmentEntry& Entry = *EntryIt;

		if (Entry.SlotTag.MatchesTagExact(EquipmentCTO->SlotTag))
		{
			RemoveEntry(Entry.Instance);
			break;
		}
	}

	FRPGEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EntryTag = EquipmentCTO->ItemTag;
	NewEntry.RarityTag = EquipmentCTO->RarityTag;
	NewEntry.EquipmentDefinition = InEquipmentDefinition;
	NewEntry.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);

	MarkItemDirty(NewEntry);
	EquipmentEntryDelegate.Broadcast(NewEntry);
	
	return NewEntry.Instance;
}

void FRPGEquipmentList::RemoveEntry(UEquipmentInstance* InEquipmentInstance)
{
	check(OwnerComponent);
	
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGEquipmentEntry& Entry = *EntryIt;

		if (Entry.Instance == InEquipmentInstance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FRPGEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	
}

void FRPGEquipmentList::PostReplicateAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		FRPGEquipmentEntry& Entry = Entries[Index];

		EquipmentEntryDelegate.Broadcast(Entry);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf(TEXT("Equipped Item: %s"), *Entry.EntryTag.ToString()));
	}
}

void FRPGEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		FRPGEquipmentEntry& Entry = Entries[Index];

		EquipmentEntryDelegate.Broadcast(Entry);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf(TEXT("UnEquipped Item: %s"), *Entry.EntryTag.ToString()));
	}
}

UEquipmentManagerComponent::UEquipmentManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentManagerComponent, EquipmentList);
}

void UEquipmentManagerComponent::EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerEquipItem(EquipmentDefinition);
		return;
	}

	EquipmentList.AddEntry(EquipmentDefinition);
}

void UEquipmentManagerComponent::UnEquipItem(UEquipmentInstance* InEquipmentInstance)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerUnEquipItem(InEquipmentInstance);
		return;
	}

	EquipmentList.RemoveEntry(InEquipmentInstance);
}

void UEquipmentManagerComponent::ServerEquipItem_Implementation(const TSubclassOf<UEquipmentDefinition>& EquipmentDefiniton)
{
	EquipItem(EquipmentDefiniton);
}

void UEquipmentManagerComponent::ServerUnEquipItem_Implementation(UEquipmentInstance* InEquipmentInstance)
{
	UnEquipItem(InEquipmentInstance);
}




