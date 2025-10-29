// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItemTypesToTables;

USTRUCT()
struct FPackedInventory
{
	GENERATED_BODY()

	virtual ~FPackedInventory() = default;
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);
	
	UPROPERTY()
	TArray<FGameplayTag> ItemTags;

	UPROPERTY()
	TArray<int32> ItemQuantities;
};
template<>
struct TStructOpsTypeTraits<FPackedInventory> : TStructOpsTypeTraitsBase2<FPackedInventory>
{
	enum
	{
		WithNetSerializer = true
	};
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryPackakdgedSignature, const FPackedInventory& /*Inventory Content*/)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGSYSTEMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UInventoryComponent();

	FOnInventoryPackakdgedSignature InventoryPackakdgedDelegate;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	void AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void UseItem(const FGameplayTag& ItemTag, int32 NumItems);
	
	UFUNCTION(BlueprintPure)
	FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag& ItemTag) const;

	void ReconstructInventory(const FPackedInventory& InInventory);
	TMap<FGameplayTag, int32>& GetInventoryMap();

	FORCEINLINE void SetOwnerLocallyControlled(const bool Value) { bOwnerLocallyControlled = Value; };
	
private:
	bool bOwnerLocallyControlled;
	
	UPROPERTY(ReplicatedUsing=OnRep_CachedInventory)
	FPackedInventory CadchedInventory;
	
	UPROPERTY(BlueprintReadOnly, meta=(allowPrivateAccess=true))
	TMap<FGameplayTag, int32> InventortyTagMap;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UItemTypesToTables> InventoryDefinitions;

	UFUNCTION(Server, Reliable)
	void ServerAddItem(const FGameplayTag& ItemTag, int32 NumItems);

	UFUNCTION(Server, Reliable)
	void ServerUseItem(const FGameplayTag& ItemTag, int32 NumItems);
	
	void PackageInventory(FPackedInventory& OutInventory);
	
	UFUNCTION()
	void OnRep_CachedInventory();
};




