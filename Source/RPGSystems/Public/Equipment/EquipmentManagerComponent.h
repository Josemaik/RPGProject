// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentManagerComponent.generated.h"

class URPGAbilitySystemComponent;
class UEquipmentInstance;
class UEquipmentDefinition;
class UEquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FRPGEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag EntryTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag SlotTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag RarityTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	TArray<FEquipmentStatEffectGroup> StatEffects = TArray<FEquipmentStatEffectGroup>();

	UPROPERTY(NotReplicated)
	FEquipmentGrantedHandles GrantedHandles = FEquipmentGrantedHandles();

	bool HasStats() const
	{
		return !StatEffects.IsEmpty();
	}
private:

	friend UEquipmentManagerComponent;
	
	friend struct FRPGEquipmentList;
	
	UPROPERTY()
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition = nullptr;

	UPROPERTY()
	TObjectPtr<UEquipmentInstance> Instance = nullptr;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipmentEntrySignature, const FRPGEquipmentEntry& /*Equipment Entry*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnEquippedEntry, const FRPGEquipmentEntry& /*UnEquiped Entry*/)

USTRUCT()
struct FRPGEquipmentList : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()

	FRPGEquipmentList() :
	OwnerComponent(nullptr)
	{}

	FRPGEquipmentList(UEquipmentManagerComponent* InComponent) :
	OwnerComponent(InComponent)
	{}

	URPGAbilitySystemComponent* GetAbilitySystemComponent();

	void AddEquipmentStats(FRPGEquipmentEntry* Entry);
	void RemoveEquipmentStats(FRPGEquipmentEntry* Entry);
	UEquipmentInstance* AddEntry(const TSubclassOf<UEquipmentDefinition>& InEquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects);
	void RemoveEntry(UEquipmentInstance* InEquipmentInstance);

	// FFastArraySerializer Contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicateAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FRPGEquipmentEntry, FRPGEquipmentList>(Entries, DeltaParams, *this);
	}

	FOnEquipmentEntrySignature EquipmentEntryDelegate;
	FOnUnEquippedEntry UnEquippedEntryDelegate;
	
	
private:
	UPROPERTY(NotReplicated)
	TObjectPtr<UEquipmentManagerComponent> OwnerComponent;
	
	UPROPERTY()
	TArray<FRPGEquipmentEntry> Entries;
};

template<>
struct TStructOpsTypeTraits<FRPGEquipmentList> : TStructOpsTypeTraitsBase2<FRPGEquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGSYSTEMS_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(Replicated)
	FRPGEquipmentList EquipmentList;
	
	UEquipmentManagerComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	
	void EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,const TArray<FEquipmentStatEffectGroup>& StatEffects);
	void UnEquipItem(UEquipmentInstance* InEquipmentInstance);

private:

	UFUNCTION(Server, Reliable) // RPC always need to copy/serialize ( const& references are not allowed)
	void ServerEquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefiniton,const TArray<FEquipmentStatEffectGroup>& StatEffects);

	UFUNCTION(Server, Reliable)
	void ServerUnEquipItem(UEquipmentInstance* InEquipmentInstance);
};

