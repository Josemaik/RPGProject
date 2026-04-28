// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentInstance.h"
#include "InventorySection/InventoryComponent.h"
#include "Net/UnrealNetwork.h"


URPGAbilitySystemComponent* FRPGEquipmentList::GetAbilitySystemComponent()
{
	check(OwnerComponent);
	check(OwnerComponent->GetOwner())

	return Cast<URPGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerComponent->GetOwner()));
}

UEquipmentInstance* FRPGEquipmentList::AddEntry(const TSubclassOf<UEquipmentDefinition>& InEquipmentDefinition,
                                                const FEquipmentEffectPackage& EffectPackage)
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
	NewEntry.SlotTag = EquipmentCTO->SlotTag;
	NewEntry.EquipmentDefinition = InEquipmentDefinition;
	NewEntry.EffectPackage = EffectPackage;
	NewEntry.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);

	NewEntry.Instance->SpawnEquipmentActors(EquipmentCTO->ActorsToSpawn,EquipmentCTO->SlotTag);
	
	if (NewEntry.HasAbility())
	{
		AddEquipmentAbility(&NewEntry);
	}
	
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
			Entry.Instance->DestroySpawnedActors(Entry.SlotTag);
			RemoveEquipmentStats(&Entry);
			RemoveEquipmentAbility(&Entry);
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FRPGEquipmentList::BindAbilitySystemDelegates()
{
	if (URPGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->OnEquipmentAbilityGiven.AddLambda(
			[this,ASC](FRPGEquipmentEntry* EquipmentEntry,bool bAsync)
		{
			CheckAbilityLevels(ASC, EquipmentEntry,bAsync);
		});
	}
}

void FRPGEquipmentList::CheckAbilityLevels(UAbilitySystemComponent* ASC, FRPGEquipmentEntry* EquipmentEntry,bool bAsync)
{
	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		for (FEquipmentAbilityGroup& AbilityGroup : EquipmentEntry->EffectPackage.Abilities)
		{
			if (Spec.GetDynamicSpecSourceTags().HasTagExact(AbilityGroup.AbilityTag))
			{
				for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
				{
					const FRPGEquipmentEntry& CurrentEntry = *EntryIt;
					
					if (!bAsync && CurrentEntry.EntryTag.MatchesTag(EquipmentEntry->EntryTag)) continue;

					if (CurrentEntry.EffectPackage.Implicit.StatEffectTag.IsValid())
					{
						const FEquipmentStatEffectGroup& ImplicitStat = CurrentEntry.EffectPackage.Implicit;
						if (CheckAbilitySingleEffect(Spec, ImplicitStat)) return;
					}
					
					for (const FEquipmentStatEffectGroup& StatEffect: CurrentEntry.EffectPackage.Prefixes)
					{
						if (CheckAbilitySingleEffect(Spec, StatEffect)) return;
					}
					for (const FEquipmentStatEffectGroup& StatEffect: CurrentEntry.EffectPackage.Suffixes)
					{
						if (CheckAbilitySingleEffect(Spec, StatEffect)) return;
					}
				}
				break;
			}
		}
	}
}

UEquipmentInstance* FRPGEquipmentList::FindInstanceByTag(FGameplayTag ItemTag)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FRPGEquipmentEntry& Entry = *EntryIt;
		if (Entry.EntryTag.MatchesTag(ItemTag)) return Entry.Instance;
	}
	return nullptr;
}

bool FRPGEquipmentList::CheckAbilitySingleEffect(FGameplayAbilitySpec& Spec, const FEquipmentStatEffectGroup& StatEffect)
{
	if (!StatEffect.ContextTag.IsValid()) return true;
	if (Spec.GetDynamicSpecSourceTags().HasTagExact(StatEffect.ContextTag))
	{
		Spec.Level = FMath::Clamp(Spec.Level + StatEffect.CurrentValue, 1.f, Spec.Level + StatEffect.CurrentValue);
	}
	return false;
}


void FRPGEquipmentList::RemoveEquipmentStats(FRPGEquipmentEntry* Entry)
{
	if (URPGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		UnEquippedEntryDelegate.Broadcast(*Entry);
		ASC->RemoveEquipmentEffects(Entry);
	}
}

void FRPGEquipmentList::AddEquipmentAbility(FRPGEquipmentEntry* Entry)
{
	if (URPGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->AddEquipmentAbility(Entry);
	}
}

void FRPGEquipmentList::RemoveEquipmentAbility(FRPGEquipmentEntry* Entry)
{
	if (URPGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveEquipmentAbility(Entry);
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

void FRPGEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
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

	if (GetOwner()->HasAuthority())
	{
		EquipmentList.BindAbilitySystemDelegates();
		EquipmentList.UnEquippedEntryDelegate.AddUObject(this, &UEquipmentManagerComponent::HandleUnEquippedItem);
	}
}

void UEquipmentManagerComponent::BindInventoryDelegates(UInventoryComponent* InvComponent)
{
	if (!IsValid(InvComponent)) return;
	InvComponent->EquipmentItemDelegate.AddUObject(this, &UEquipmentManagerComponent::HandleEquipmentRequested);
	InvComponent->OnEquipmentItemDroppedDelegate.AddUObject(this, &UEquipmentManagerComponent::UnEquipItemByTag);
	InvComponentRef = InvComponent;
}

void UEquipmentManagerComponent::EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
                                           const FEquipmentEffectPackage& EffectPackage)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerEquipItem(EquipmentDefinition,EffectPackage);
		return;
	}

	CurrentUnequipReason = EUnequipReason::Swap;
	
	if (UEquipmentInstance* Result = EquipmentList.AddEntry(EquipmentDefinition,EffectPackage))
	{
		Result->OnUnEquipped();
	}
	
	CurrentUnequipReason = EUnequipReason::Manual;
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

void UEquipmentManagerComponent::UnEquipItemByTag(FGameplayTag ItemTag,EUnequipReason Reason)
{
	CurrentUnequipReason = Reason;
	
	if (!GetOwner()->HasAuthority())
	{
		ServerUnEquipItemByTag(ItemTag,Reason);
		return;
	}

	if (UEquipmentInstance* Instance = EquipmentList.FindInstanceByTag(ItemTag))
	{
		UnEquipItem(Instance);
	}
	
	CurrentUnequipReason = EUnequipReason::Manual;
}

void UEquipmentManagerComponent::HandleEquipmentRequested(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
	const FEquipmentEffectPackage& EffectPackage)
{
	EquipItem(EquipmentDefinition, EffectPackage);
}

void UEquipmentManagerComponent::HandleUnEquippedItem(const FRPGEquipmentEntry& UnEquippedEntry) const
{
	if (CurrentUnequipReason != EUnequipReason::Manual) return;
	if (IsValid(InvComponentRef))
	{
		InvComponentRef->AddUnEquippedItemEntry(UnEquippedEntry.EntryTag, UnEquippedEntry.EffectPackage);
	}
}

void UEquipmentManagerComponent::ServerEquipItem_Implementation(TSubclassOf<UEquipmentDefinition> EquipmentDefiniton,
                                                                const FEquipmentEffectPackage& EffectPackage)
{
	EquipItem(EquipmentDefiniton,EffectPackage);
}

void UEquipmentManagerComponent::ServerUnEquipItem_Implementation(UEquipmentInstance* InEquipmentInstance)
{
	UnEquipItem(InEquipmentInstance);
}

void UEquipmentManagerComponent::ServerUnEquipItemByTag_Implementation(FGameplayTag ItemTag,EUnequipReason Reason)
{
	UnEquipItemByTag(ItemTag,Reason);
}




