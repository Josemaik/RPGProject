// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentInstance.h"
#include "Net/UnrealNetwork.h"


URPGAbilitySystemComponent* FRPGEquipmentList::GetAbilitySystemComponent()
{
	check(OwnerComponent);
	check(OwnerComponent->GetOwner())

	return Cast<URPGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerComponent->GetOwner()));
}

UEquipmentInstance* FRPGEquipmentList::AddEntry(const TSubclassOf<UEquipmentDefinition>& InEquipmentDefinition,
                                                const TArray<FEquipmentStatEffectGroup>& StatEffects)
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
	NewEntry.SlotTag = EquipmentCTO->SlotTag;
	NewEntry.EquipmentDefinition = InEquipmentDefinition;
	NewEntry.StatEffects = StatEffects;
	NewEntry.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);

	NewEntry.Instance->SpawnEquipmentActors(EquipmentCTO->ActorsToSpawn);
	
	if (NewEntry.HasStats())
	{
		AddEquipmentStats(&NewEntry);
	}
	
	MarkItemDirty(NewEntry);
	
	EquipmentEntryDelegate.Broadcast(NewEntry);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf(TEXT("Equipped Item: %s"), *NewEntry.EntryTag.ToString()));
	
	return NewEntry.Instance;
}

void FRPGEquipmentList::AddEquipmentStats(FRPGEquipmentEntry* Entry)
{
	if (URPGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->AddEquipmentEffects(Entry);
	}
}

void FRPGEquipmentList::RemoveEntry(UEquipmentInstance* InEquipmentInstance)
{
	check(OwnerComponent);
	
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGEquipmentEntry& Entry = *EntryIt;

		if (Entry.Instance == InEquipmentInstance)
		{
			Entry.Instance->DestroySpawnedActors();
			RemoveEquipmentStats(&Entry);
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}


void FRPGEquipmentList::RemoveEquipmentStats(FRPGEquipmentEntry* Entry)
{
	if (URPGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		UnEquippedEntryDelegate.Broadcast(*Entry);
		ASC->RemoveEquipmentEffects(Entry);
	}
}

void FRPGEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		const FRPGEquipmentEntry& Entry = Entries[Index];
		
		EquipmentEntryDelegate.Broadcast(Entry);
		
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf(TEXT("UnEquipped Item: %s"), *Entry.EntryTag.ToString()));
	}
}

void FRPGEquipmentList::PostReplicateAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		const FRPGEquipmentEntry& Entry = Entries[Index];
		
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
	}
}

UEquipmentManagerComponent::UEquipmentManagerComponent() :
	EquipmentList(this)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentManagerComponent, EquipmentList);
}

void UEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Equipment Owner: %s"), *GetOwner()->GetName());
}

void UEquipmentManagerComponent::EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
                                           const TArray<FEquipmentStatEffectGroup>& StatEffects)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerEquipItem(EquipmentDefinition,StatEffects);
		return;
	}

	if (UEquipmentInstance* Result = EquipmentList.AddEntry(EquipmentDefinition,StatEffects))
	{
		Result->OnUnEquipped();
	}
}

void UEquipmentManagerComponent::UnEquipItem(UEquipmentInstance* InEquipmentInstance)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerUnEquipItem(InEquipmentInstance);
		return;
	}
	
	InEquipmentInstance->OnUnEquipped();
	EquipmentList.RemoveEntry(InEquipmentInstance);
}

void UEquipmentManagerComponent::ServerEquipItem_Implementation(TSubclassOf<UEquipmentDefinition> EquipmentDefiniton,
	const TArray<FEquipmentStatEffectGroup>& StatEffects)
{
	EquipItem(EquipmentDefiniton,StatEffects);
}

void UEquipmentManagerComponent::ServerUnEquipItem_Implementation(UEquipmentInstance* InEquipmentInstance)
{
	UnEquipItem(InEquipmentInstance);
}




