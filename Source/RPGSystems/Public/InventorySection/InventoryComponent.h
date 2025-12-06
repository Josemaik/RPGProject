// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "Components/ActorComponent.h"
#include "Equipment/EquipmentTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryComponent.generated.h"


class UEquipmentStaffEfects;
class UItemTypesToTables;
class UInventoryComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FEquipmentItemUsed, const TSubclassOf<UEquipmentDefinition>& /*Equipment Definition*/,const FEquipmentEffectPackage& /* Stat Effects and one Ability */)

USTRUCT(BlueprintType)
struct FRPGInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FText ItemName = FText();
	
	UPROPERTY(BlueprintReadOnly)
	int Quantity = 0;

	UPROPERTY(BlueprintReadOnly)
	int64 ItemID = 0;

	UPROPERTY(BlueprintReadOnly)
	FEquipmentEffectPackage EffectPackage = FEquipmentEffectPackage();

	FORCEINLINE bool IsValid() const { return ItemID != 0; }

	bool operator==(const FRPGInventoryEntry& Other) const
	{
		return Other.ItemID == ItemID;
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FDirtyInventoryItemsSignature, const FRPGInventoryEntry&  /*Dirty Item*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemRemovedSignature, const int64 /*Item ID*/)

USTRUCT()
struct FRPGInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()
	
	FRPGInventoryList() :
	OwnerComponent(nullptr)
	{}

	FRPGInventoryList(UInventoryComponent* InOwnerComponent) :
	OwnerComponent(InOwnerComponent)
	{}

	void AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);
	void RemoveItem(const FRPGInventoryEntry& Entry, int32 NumItems = 1);
	bool HasEnough(const FGameplayTag& ItemTag, int32 NumItems = 1);
	uint64 GenerateID();
	void SetStats(UEquipmentStaffEfects* InStats);
	void RollForStats(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, FRPGInventoryEntry* Entry);
	void AddUnEquippedItem(const FGameplayTag& ItemTag, const FEquipmentEffectPackage& EffectPackage);
	
	// FFastArraySerializwer Contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FRPGInventoryEntry, FRPGInventoryList>(Entries,DeltaParams,*this);
	}
	
	FDirtyInventoryItemsSignature DirtyItemDelegate;
	FInventoryItemRemovedSignature InventoryItemRemovedDelegate;
private:
	friend class UInventoryComponent;
	
	UPROPERTY()
	TArray<FRPGInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UInventoryComponent> OwnerComponent;

	UPROPERTY(NotReplicated)
	uint64 LastAssingID = 0;

	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UEquipmentStaffEfects> WeakStats;
};

template<>
struct TStructOpsTypeTraits<FRPGInventoryList> : TStructOpsTypeTraitsBase2<FRPGInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGSYSTEMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FEquipmentItemUsed EquipmentItemDelegate;
	
	UPROPERTY(Replicated,ReplicatedUsing = OnRep_InventoryList)
	FRPGInventoryList InventoryList;

	UFUNCTION()
	void OnRep_InventoryList();
	
	UInventoryComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void UseItem(const FRPGInventoryEntry& Entry, int32 NumItems);
	
	UFUNCTION(BlueprintPure)
	FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag& ItemTag) const;

	TArray<FRPGInventoryEntry> GetInventoryEntries();

	void AddUnEquippedItemEntry(const FGameplayTag& ItemTag, const FEquipmentEffectPackage& EffectPackage);

	UFUNCTION(BlueprintPure)
	TArray<FRPGInventoryEntry> GetEntriesByString(const FString& InString);
private:
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Stat Effect")
	TObjectPtr<UEquipmentStaffEfects> StatEffects;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Item Definitions")
	TObjectPtr<UItemTypesToTables> InventoryDefinitions;
	
	//Server methods
	UFUNCTION(Server, Reliable)
	void ServerAddItem(const FGameplayTag& ItemTag, int32 NumItems);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem(const FRPGInventoryEntry& Entry, int32 NumItems);

	bool ServerUseItem_Validate(const FRPGInventoryEntry& Entry, int32 NumItems);
};



