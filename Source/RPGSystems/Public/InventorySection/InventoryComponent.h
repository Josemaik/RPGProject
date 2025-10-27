// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

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


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGSYSTEMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	void AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);
	
private:
	UPROPERTY(ReplicatedUsing=OnRep_CachedInventory)
	FPackedInventory CadchedInventory;
	
	UPROPERTY(BlueprintReadOnly, meta=(allowPrivateAccess=true))
	TMap<FGameplayTag, int32> InventortyTagMap;

	UFUNCTION(Server, Reliable)
	void ServerAddItem(const FGameplayTag& ItemTag, int32 NumItems);

	void PackageInventory(FPackedInventory& OutInventory);
	void ReconstructInventory(const FPackedInventory& InInventory);
	
	UFUNCTION()
	void OnRep_CachedInventory();
};


