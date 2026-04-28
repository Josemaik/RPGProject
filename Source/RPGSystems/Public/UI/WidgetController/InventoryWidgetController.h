// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/WidgetController.h"
#include "InventoryWidgetController.generated.h"

class UEquipmentManagerComponent;
struct FGameplayTag;
struct FRPGInventoryEntry;
struct FMasterItemDefinition;
class UInventoryComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryEntrySignature, const FRPGInventoryEntry&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryItemRemoeved, const FRPGInventoryEntry&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryWeightChanged, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSortItemsRequested,bool);
DECLARE_MULTICAST_DELEGATE(FOnDropKeyPressed);
DECLARE_MULTICAST_DELEGATE(FOnEquipKeyPressed);

/**
 * This class comunicates Component logic <-> UI
 */
UCLASS(Blueprintable, BlueprintType)
class RPGSYSTEMS_API UInventoryWidgetController : public UWidgetController
{
	GENERATED_BODY()
public:
	FInventoryEntrySignature InventoryEntryDelegate;
	FOnInventoryItemRemoeved OnInventoryItemRemoved;
	FOnInventoryWeightChanged OnInventoryWeightChanged;
	FOnSortItemsRequested OnSortItemsRequested;
	FOnDropKeyPressed OnDropKeyPressed;
	FOnEquipKeyPressed OnEquipKeyPressed;
	
	void SetOwningActor(AActor* InOwner);
	FORCEINLINE AActor* GetOwningActor() { return OwningActor; };

	void BindCallbacksToDependencies();
	void BroadCastInitialValues() const;

	//Inventory Component
	void EquipItem(const FRPGInventoryEntry& Entry) const;
	void AddEquippedItem(FGameplayTag ItemTag,uint64 ExistingID) const;

	//Equipment Component
	void UnEquipItem(const FGameplayTag ItemTag) const;

	//Drop Panel
	void DropItemToWorld(const FRPGInventoryEntry& Entry) const;

	//Input sorts
	void RequestSortItems(bool Quick) const;

	float GetMaxInventoryWeight() const;
	const FMasterItemDefinition GetInventoryItemDefinition(FGameplayTag ItemTag) const;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> OwningActor;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;

	UPROPERTY()
	TObjectPtr<UEquipmentManagerComponent> OwningEquipment;
};
