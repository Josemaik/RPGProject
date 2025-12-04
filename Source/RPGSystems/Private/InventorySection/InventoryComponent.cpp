// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySection/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "PropertyCustomizationHelpers.h"
#include "Data/EquipmentStaffEfects.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentTypes.h"
#include "InventorySection/ItemTypesToTables.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"

namespace FGameplayTags::Static
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Category_Equipment, "Item.Equipment");
}

///////////////////////////////
//* UInventoryList Methods
///////////////////////////////

void FRPGInventoryList::AddItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	if (ItemTag.MatchesTag(FGameplayTags::Static::Category_Equipment))
	{
		//cannot stack these categories, do nothing	
	}
	else
	{
		for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
		{
			FRPGInventoryEntry& Entry = *EntryIt;

			if (Entry.ItemTag.MatchesTagExact(ItemTag))
			{
				Entry.Quantity += NumItems;

				MarkItemDirty(Entry);
				if (OwnerComponent->GetOwner()->HasAuthority())
				{
					DirtyItemDelegate.Broadcast(Entry);
				}
				return;
			}
		}
	}
	
	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);
	
	FRPGInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.ItemName = Item.ItemName;
	NewEntry.Quantity = NumItems;
	NewEntry.ItemID = GenerateID();

	if (NewEntry.ItemTag.MatchesTag(FGameplayTags::Static::Category_Equipment) && IsValid(WeakStats.Get()))
	{
		RollForStats(Item.EquipmentItemProps.EquipmentClass, &NewEntry);
	}
	
	
	MarkItemDirty(Entries.Last());
	if (OwnerComponent->GetOwner()->HasAuthority())
	{
		DirtyItemDelegate.Broadcast(NewEntry);
	}
}

void FRPGInventoryList::RollForStats(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
	FRPGInventoryEntry* Entry)
{
	if (!IsValid(EquipmentDefinition))
	{
		return;
	}
	
	UEquipmentStaffEfects* StatEffects = WeakStats.Get();
	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);
	
	if (EquipmentCDO->bForceAbilityRoll || FMath::FRandRange(0.f, 1.f) < EquipmentCDO->ProbabilityToRollAbility)
	{
		bool bShouldRoll = true;
		while (bShouldRoll)
		{
			const int32 RandomIndex = FMath::RandRange(0, EquipmentCDO->PossibleAbilityRoles.Num() - 1);
			const FGameplayTag& RandomTag = EquipmentCDO->PossibleAbilityRoles.GetByIndex(RandomIndex);

			for (const auto& Pair :StatEffects->MasterStatMap)
			{
				if (RandomTag.MatchesTag(Pair.Key))
				{
					if (const FEquipmentAbilityGroup* PossibleAbility = URPGAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentAbilityGroup>(Pair.Value, RandomTag))
					{
						if (FMath::RandRange(0.f, 1.f) <= PossibleAbility->ProabilityToSelect)
						{
							Entry->EffectPackage.Ability = *PossibleAbility;
							bShouldRoll = false;
							break;
						}
					}
				}
			}
		}
	}
	
	const int32 NumStatsToRoll = FMath::RandRange(EquipmentCDO->MinPossibleStats, EquipmentCDO->MaxPossibleStats);
	int32 StatRollIndex = 0;
	FGameplayTagContainer PossibleStatContainer = EquipmentCDO->PossibleStatsRoles;
	while (StatRollIndex < NumStatsToRoll)
	{
		const int32 RandomIndex = FMath::RandRange(0, PossibleStatContainer.Num() - 1);
		const FGameplayTag& RandomTag = PossibleStatContainer.GetByIndex(RandomIndex);

		for (const auto& Pair : StatEffects->MasterStatMap)
		{
			if (RandomTag.MatchesTag(Pair.Key))
			{
				if (const FEquipmentStatEffectGroup* PossibleStat = URPGAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentStatEffectGroup>(Pair.Value, RandomTag))
				{
					if (FMath::FRandRange(0.f, 1.f) < PossibleStat->ProbabilityToSelect)
					{
						FEquipmentStatEffectGroup NewStat = *PossibleStat;

						NewStat.CurrentValue = PossibleStat->bFractionalStat ? FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel) :
						FMath::TruncToInt(FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel));

						Entry->EffectPackage.StatEffects.Add(NewStat);
						++StatRollIndex;
						
						// make unique stats
						PossibleStatContainer.RemoveTag(RandomTag); 
						FName RandomTagName = RandomTag.GetTagLeafName(); //Stats.LowLevel.Health => Health
						FGameplayTagContainer TagsToRemove;
						
						for (const FGameplayTag& Tag : PossibleStatContainer)
						{
							if (Tag.GetTagLeafName() == RandomTagName)
							{
								TagsToRemove.AddTag(Tag);
							}
						}
						
						PossibleStatContainer.RemoveTags(TagsToRemove);
						
						break;
					}
				}
			}
		}
	}
}

void FRPGInventoryList::AddUnEquippedItem(const FGameplayTag& ItemTag,
	const FEquipmentEffectPackage& EffectPackage)
{
	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);
	
	FRPGInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.ItemName = Item.ItemName;
	NewEntry.Quantity = 1;
	NewEntry.ItemID = GenerateID();
	NewEntry.EffectPackage = EffectPackage;
	
	MarkItemDirty(NewEntry);
	DirtyItemDelegate.Broadcast(NewEntry);
	UE_LOG(LogTemp, Warning, TEXT("[InventoryList] AddUnEquippedItem MarkItemDirty called for ItemID %llu"), NewEntry.ItemID);
}


void FRPGInventoryList::RemoveItem(const FRPGInventoryEntry& InventoryEntry, int32 NumItems)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGInventoryEntry& Entry = *EntryIt;
		
		if (Entry.ItemID == InventoryEntry.ItemID)
		{
			Entry.Quantity -= NumItems;

			if (Entry.Quantity > 0)
			{
				MarkItemDirty(Entry);

				if (OwnerComponent->GetOwner()->HasAuthority())
				{
					DirtyItemDelegate.Broadcast(Entry);
				}
			}
			else
			{
				InventoryItemRemovedDelegate.Broadcast(Entry.ItemID);
				EntryIt.RemoveCurrent();
				MarkArrayDirty();
			}
			break;
		}
	}
}

bool FRPGInventoryList::HasEnough(const FGameplayTag& ItemTag, int32 NumItems)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemTag.MatchesTagExact(ItemTag))
		{
			if (Entry.Quantity >= NumItems)
			{
				return true;
			}
		}
	}
	return false;
}

uint64 FRPGInventoryList::GenerateID()
{
	uint64 NewID = ++LastAssingID;

	int32 SignatureIndex = 0;
	while (SignatureIndex < 12)
	{
		if (FMath::RandRange(0,100) < 85)
		{
			NewID |= (uint64)1 << FMath::RandRange(0,63);
		}
		++SignatureIndex;
	}
	
	return NewID;
}

void FRPGInventoryList::SetStats(UEquipmentStaffEfects* InStats)
{
	WeakStats = InStats;
}


void FRPGInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FRPGInventoryEntry& Entry = Entries[Index];
		InventoryItemRemovedDelegate.Broadcast(Entry.ItemID);
	}
}

void FRPGInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FRPGInventoryEntry& Entry = Entries[Index];
		DirtyItemDelegate.Broadcast(Entry);
	}
}

void FRPGInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FRPGInventoryEntry& Entry = Entries[Index];
		DirtyItemDelegate.Broadcast(Entry);
	}
}

///////////////////////////////
//* UInventoryComponent Methods
///////////////////////////////

void UInventoryComponent::OnRep_InventoryList()
{
	InventoryList.OwnerComponent = this;
}

UInventoryComponent::UInventoryComponent() :
	InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InventoryList.OwnerComponent = this;
	
	if (GetOwner()->HasAuthority())
	{
		InventoryList.SetStats(StatEffects);
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
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
	}

	InventoryList.AddItem(ItemTag, NumItems);
	GetOwner()->ForceNetUpdate();
}

void UInventoryComponent::UseItem(const FRPGInventoryEntry& Entry, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	if (!Owner->HasAuthority())
	{
		ServerUseItem(Entry,NumItems);
		return;
	}
	
	if (InventoryList.HasEnough(Entry.ItemTag, NumItems))
	{
		const FMasterItemDefinition Item = GetItemDefinitionByTag(Entry.ItemTag);
		
		
		if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
		{
			if (IsValid(Item.ConsumableProps.ItemEffectClass))
			{
				const FGameplayEffectContextHandle ContextHandle = OwnerASC->MakeEffectContext();
				const FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(Item.ConsumableProps.ItemEffectClass,
				Item.ConsumableProps.ItemEffectLevel,ContextHandle);
				OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				InventoryList.RemoveItem(Entry);

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta,FString::Printf(TEXT("Server Item ussed: %s"),
					*Item.ItemTag.ToString()));
			}
			if (IsValid(Item.EquipmentItemProps.EquipmentClass))
			{
				EquipmentItemDelegate.Broadcast(Item.EquipmentItemProps.EquipmentClass, Entry.EffectPackage);
				InventoryList.RemoveItem(Entry);
			}
		}
	}
}

FMasterItemDefinition UInventoryComponent::GetItemDefinitionByTag(const FGameplayTag& ItemTag) const
{
	checkf(InventoryDefinitions, TEXT("No inventory definitions inside component: %s"),*GetNameSafe(this));

	for (const auto& Pair : InventoryDefinitions->TagsToTables)
	{
		if (ItemTag.MatchesTag(Pair.Key))
		{
			if (const FMasterItemDefinition* ValidItem = URPGAbilitySystemLibrary::GetDataTableRowByTag<FMasterItemDefinition>(Pair.Value, ItemTag))
			{
				return *ValidItem;
			}
		}
	}
	return FMasterItemDefinition();
}

TArray<FRPGInventoryEntry> UInventoryComponent::GetInventoryEntries()
{
	return InventoryList.Entries;
}

void UInventoryComponent::AddUnEquippedItemEntry(const FGameplayTag& ItemTag,
	const FEquipmentEffectPackage& EffectPackage)
{
	InventoryList.AddUnEquippedItem(ItemTag, EffectPackage);
}

//////////////////////////////
//* Server Methods
//////////////////////////////

void UInventoryComponent::ServerAddItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems)
{
	AddItem(ItemTag,NumItems);
}

void UInventoryComponent::ServerUseItem_Implementation(const FRPGInventoryEntry& Entry, int32 NumItems)
{
	UseItem(Entry, NumItems);
}

bool UInventoryComponent::ServerUseItem_Validate(const FRPGInventoryEntry& Entry, int32 NumItems)
{
	return Entry.IsValid() && InventoryList.HasEnough(Entry.ItemTag, NumItems);
}

