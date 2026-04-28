// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySection/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Data/EquipmentStaffEfects.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentGenerator.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Equipment/EquipmentTypes.h"
#include "InventorySection/ItemActor.h"
#include "InventorySection/ItemTypesToTables.h"
#include "Libraries/RPGAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "RPGSystems/RPGSystems.h"


///////////////////////////////
//* UInventoryList Methods
///////////////////////////////

FRPGInventoryEntry* FRPGInventoryList::AddItem(const FGameplayTag& ItemTag,uint64 ExistingID, int32 NumItems)
{
	if (ItemTag.MatchesTag(RPGInventoryTags::ItemsCategory::Equipment))
	{
		//cannot stack these categories, do nothing	
	}
	else
	{
		 if (FRPGInventoryEntry* StackedEntry = TryStackItem(ItemTag, NumItems))
		 {
			 return StackedEntry;
		 }
	}
	
	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);
	
	FRPGInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.Quantity = NumItems;
	NewEntry.ItemID = (ExistingID != 0) ? ExistingID : GenerateID();

	if (NewEntry.ItemTag.MatchesTag(RPGInventoryTags::ItemsCategory::Equipment) && IsValid(WeakStats.Get()))
	{
		UEquipmentGenerator::RollForStats(NewEntry.EffectPackage,Item.EquipmentItemProps.EquipmentClass,WeakStats.Get());
	}
	
	
	MarkItemDirty(Entries.Last());
	if (OwnerComponent->GetOwner()->HasAuthority())
	{
		DirtyItemDelegate.Broadcast(NewEntry);
	}

	return &NewEntry;
}

FRPGInventoryEntry* FRPGInventoryList::TryStackItem(const FGameplayTag& ItemTag, int32 NumItems)
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
			return &Entry;
		}
	}
	return nullptr;
}

void FRPGInventoryList::AddUnEquippedItem(const FGameplayTag& ItemTag,
                                          const FEquipmentEffectPackage& EffectPackage,int32 NumItems)
{
	if (!ItemTag.MatchesTag(RPGInventoryTags::ItemsCategory::Equipment))
	{   //if not an equipment try to stack
		if (TryStackItem(ItemTag, NumItems)) return;
	}
	
	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);
	
	FRPGInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.Quantity = NumItems;
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
				InventoryItemRemovedDelegate.Broadcast(Entry);
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
		InventoryItemRemovedDelegate.Broadcast(Entry);
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
	
	InventoryList.DirtyItemDelegate.AddLambda([this](const FRPGInventoryEntry&){ UpdateWeight(); });
	InventoryList.InventoryItemRemovedDelegate.AddLambda([this](const FRPGInventoryEntry&){ UpdateWeight(); });
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
}

void UInventoryComponent::AddItem(const FGameplayTag& ItemTag,int64 ExistingID, int32 NumItems)
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

	InventoryList.AddItem(ItemTag,ExistingID, NumItems);
	
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
				EquipmentItemDelegate.Broadcast(Item.EquipmentItemProps.EquipmentClass,Entry.EffectPackage);
				InventoryList.RemoveItem(Entry);
			}
		}
	}
}

void UInventoryComponent::DropItem(const FRPGInventoryEntry& Entry, int32 NumItems)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerDropItem(Entry, NumItems);
		return;
	}

	if (Entry.ItemTag.MatchesTag(RPGInventoryTags::ItemsCategory::Equipment))
	{
		OnEquipmentItemDroppedDelegate.Broadcast(Entry.ItemTag,EUnequipReason::Drop);
	}

	//ItemDroppedDelegate.Broadcast(&Entry, NumItems);
	SpawnDroppedItem(Entry, NumItems);
	InventoryList.RemoveItem(Entry,NumItems);
}

void UInventoryComponent::SpawnDroppedItem(const FRPGInventoryEntry& DroppedEntry, int32 NumItems)
{
	if (!DroppedEntry.IsValid())
	{
		return;
	}

	const TObjectPtr<APawn> OwnerPawn = Cast<APawn>(InventoryList.OwningObject);
	if (!IsValid(OwnerPawn))
	{
		return;
	}
	
	const FVector FordwardLocation = OwnerPawn->GetActorLocation() + OwnerPawn->GetActorForwardVector() * ItemSpawnFordwardDistance;
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(FordwardLocation);

	SpawnItem(SpawnTransform, &DroppedEntry, NumItems);
}


void UInventoryComponent::PickupItem(AItemActor* Item)
{
	if (!IsValid(Item)) return;

	if (!GetOwner()->HasAuthority())
	{
		ServerPickupItem(Item);
		return;
	}

	InventoryList.AddUnEquippedItem(Item->ItemTag,Item->EffectPackage, Item->NumItems);
	Item->Destroy();
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

TArray<FRPGInventoryEntry> UInventoryComponent::GetEntriesByString(const FString& InString)
{
	TArray<FRPGInventoryEntry> MatchEntries;
	for (auto EntryIt = InventoryList.Entries.CreateConstIterator(); EntryIt; ++EntryIt)
	{
		const FRPGInventoryEntry& Entry = *EntryIt;
		const FMasterItemDefinition& EntryDefinition = GetItemDefinitionByTag(Entry.ItemTag);
		if (EntryDefinition.ItemName.ToString().Contains(InString))
		{
			MatchEntries.Add(Entry);
		}
		
		for (const FEquipmentAbilityGroup& AbilityGroup : Entry.EffectPackage.Abilities)
		{
			if (AbilityGroup.AbilityName.ToString().Contains(InString))
			{
				if (!MatchEntries.Contains(Entry))
				{
					MatchEntries.Add(Entry);
					break;
				}
			}
		}

		if (Entry.EffectPackage.Implicit.StatEffectTag.IsValid())
		{
			if (Entry.EffectPackage.Implicit.StatEffectName.ToString().Contains(InString))
			{
				if (!MatchEntries.Contains(Entry))
				{
					MatchEntries.Add(Entry);
				}
			}
		}

		for (const FEquipmentStatEffectGroup& StatEffect : Entry.EffectPackage.Prefixes)
		{
			if (StatEffect.StatEffectName.ToString().Contains(InString))
			{
				if (!MatchEntries.Contains(Entry))
				{
					MatchEntries.Add(Entry);
				}
			}
		}

		for (const FEquipmentStatEffectGroup& StatEffect : Entry.EffectPackage.Suffixes)
		{
			if (StatEffect.StatEffectName.ToString().Contains(InString))
			{
				if (!MatchEntries.Contains(Entry))
				{
					MatchEntries.Add(Entry);
				}
			}
		}
	}

	return MatchEntries;
}

void UInventoryComponent::SpawnItem(const FTransform& SpawnTransform, const FRPGInventoryEntry* DroppedEntry, int32 NumItems)
{
	if (!IsValid(DefaultItemClass)) return;
	
	AItemActor* NewActor = GetWorld()->SpawnActorDeferred<AItemActor>(DefaultItemClass, SpawnTransform);
	if (IsValid(NewActor))
	{
		NewActor->SetParams(DroppedEntry, NumItems);
		NewActor->ValidationBits |= SERVER_BITS;
		
		FMasterItemDefinition Item = GetItemDefinitionByTag(DroppedEntry->ItemTag);
		if (IsValid(Item.ItemMesh.Get()))
		{
			NewActor->SetMesh(Item.ItemMesh.Get());
			NewActor->FinishSpawning(SpawnTransform);
		}
		else
		{
			FStreamableManager& Manager = UAssetManager::GetStreamableManager();
			Manager.RequestAsyncLoad(Item.ItemMesh.ToSoftObjectPath(),
				[NewActor, Item, SpawnTransform]
				{
					NewActor->SetMesh(Item.ItemMesh.Get());
					NewActor->FinishSpawning(SpawnTransform);
				});
		}
	}
}

void UInventoryComponent::UpdateWeight()
{
	CurrentWeight = 0.f;

	for (auto& Item : InventoryList.GetEntries())
	{
		const FMasterItemDefinition& EntryDefinition = GetItemDefinitionByTag(Item.ItemTag);
		CurrentWeight += EntryDefinition.Weight * static_cast<float>(Item.Quantity);
	}

	OnWeightChanged.Broadcast(CurrentWeight);
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

bool UInventoryComponent::ServerPickupItem_Validate(AItemActor* Item)
{
	return Item->ValidationBits & SERVER_BITS;
}

void UInventoryComponent::ServerPickupItem_Implementation(AItemActor* Item)
{
	PickupItem(Item);
}

void UInventoryComponent::ServerDropItem_Implementation(const FRPGInventoryEntry& Entry, int32 NumItems)
{
	DropItem(Entry, NumItems);
}

