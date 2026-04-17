// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/WidgetController.h"
#include "InventoryWidgetController.generated.h"

struct FRPGInventoryEntry;
struct FMasterItemDefinition;
class UInventoryComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryEntrySignature, const FRPGInventoryEntry&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryItemRemoeved, const FRPGInventoryEntry&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryWeightChanged, float);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPGSYSTEMS_API UInventoryWidgetController : public UWidgetController
{
	GENERATED_BODY()
public:

	
	FInventoryEntrySignature InventoryEntryDelegate;
	
	FOnInventoryItemRemoeved OnInventoryItemRemoved;
	FOnInventoryWeightChanged OnInventoryWeightChanged;
	
	void SetOwningActor(AActor* InOwner);
	FORCEINLINE AActor* GetOwningActor() { return OwningActor; };

	void BindCallbacksToDependencies();
	void BroadCastInitialValues() const;
	
private:
	UPROPERTY()
	TObjectPtr<AActor> OwningActor;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;
};
