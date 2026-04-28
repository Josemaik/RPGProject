// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentTypes.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentManagerComponent.generated.h"

UENUM()
enum class EUnequipReason : uint8
{
	Manual,  // double clikc
	Swap,    // swapped by other item
	Drop     // drop to world
};

class URPGAbilitySystemComponent;
class UEquipmentInstance;
class UEquipmentDefinition;
class UEquipmentManagerComponent;
class UInventoryComponent;

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
	FEquipmentEffectPackage EffectPackage = FEquipmentEffectPackage();

	UPROPERTY(NotReplicated)
	FEquipmentGrantedHandles GrantedHandles = FEquipmentGrantedHandles();

	bool HasStats() const
	{
		return !EffectPackage.Prefixes.IsEmpty() || !EffectPackage.Suffixes.IsEmpty() ||
			EffectPackage.Implicit.StatEffectTag.IsValid() || EffectPackage.BaseDamage.StatEffectTag.IsValid();
	}

	bool HasAbility() const
	{
		for (auto& Ability : EffectPackage.Abilities)
		{
			if (Ability.AbilityTag.IsValid()) return true;
		}
		return false;
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
	void AddEquipmentAbility(FRPGEquipmentEntry* Entry);
	void RemoveEquipmentAbility(FRPGEquipmentEntry* Entry);
	UEquipmentInstance* AddEntry(const TSubclassOf<UEquipmentDefinition>& InEquipmentDefinition,const FEquipmentEffectPackage& EffectPackage);
	void RemoveEntry(UEquipmentInstance* InEquipmentInstance);
	void BindAbilitySystemDelegates();
	bool CheckAbilitySingleEffect(FGameplayAbilitySpec& Spec, const FEquipmentStatEffectGroup& StatEffect);
	void CheckAbilityLevels(UAbilitySystemComponent* ASC, FRPGEquipmentEntry* EquipmentEntry,bool bAsync);

	UEquipmentInstance* FindInstanceByTag(FGameplayTag ItemTag);
	
	// FFastArraySerializer Contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
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
	
	void EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,const FEquipmentEffectPackage& EffectPackage);
	void UnEquipItem(UEquipmentInstance* InEquipmentInstance);
	void UnEquipItemByTag(FGameplayTag ItemTag,EUnequipReason Reason);
	void HandleEquipmentRequested(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
	const FEquipmentEffectPackage& EffectPackage);
	void HandleUnEquippedItem(const FRPGEquipmentEntry& UnEquippedEntry) const;

	void BindInventoryDelegates(UInventoryComponent* InvComponent);
protected:
	virtual void BeginPlay() override;

private:
	EUnequipReason CurrentUnequipReason = EUnequipReason::Manual;
	
	bool bIsSwappingEquipment = false;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InvComponentRef;

	UFUNCTION(Server, Reliable) // RPC always need to copy/serialize ( const& references are not allowed)
	void ServerEquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefiniton,const FEquipmentEffectPackage& EffectPackage);

	UFUNCTION(Server, Reliable)
	void ServerUnEquipItem(UEquipmentInstance* InEquipmentInstance);

	UFUNCTION(Server, Reliable)
	void ServerUnEquipItemByTag(FGameplayTag ItemTag,EUnequipReason Reason);
};



