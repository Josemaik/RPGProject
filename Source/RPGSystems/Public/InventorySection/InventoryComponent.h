// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryComponent.generated.h"

class UItemTypesToTables;

USTRUCT(BlueprintType)
struct FRPGInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	int Quantity = 0;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FDirtyInventoryItemsSignature, const FRPGInventoryEntry&  /*Dirty Item*/)

USTRUCT()
struct FRPGInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()
	FRPGInventoryList() :
	OwnerComponent(nullptr)
	{}

	FRPGInventoryList(UActorComponent* InOwnerComponent) :
	OwnerComponent(InOwnerComponent)
	{}

	void AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);
	void RemoveItem(const FGameplayTag& ItemTag, int32 NumItems = 1);
	bool HasEnough(const FGameplayTag& ItemTag, int32 NumItems = 1);

	// FFastArraySerializwer Contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicateAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FRPGInventoryEntry, FRPGInventoryList>(Entries,DeltaParams,*this);
	}

	FDirtyInventoryItemsSignature DirtyItemDelegate;
private:
	friend class UInventoryComponent;
	UPROPERTY()
	TArray<FRPGInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
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

	UPROPERTY(Replicated)
	FRPGInventoryList InventoryList;
	
	UInventoryComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	void AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void UseItem(const FGameplayTag& ItemTag, int32 NumItems);
	
	UFUNCTION(BlueprintPure)
	FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag& ItemTag) const;

	TArray<FRPGInventoryEntry> GetInventoryEntries() const;
private:
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UItemTypesToTables> InventoryDefinitions;

	UFUNCTION(Server, Reliable)
	void ServerAddItem(const FGameplayTag& ItemTag, int32 NumItems);

	UFUNCTION(Server, Reliable)
	void ServerUseItem(const FGameplayTag& ItemTag, int32 NumItems);
};



