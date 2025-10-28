// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/WidgetController.h"
#include "InventoryWidgetController.generated.h"

struct FMasterItemDefinition;
struct FPackedInventory;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemSignature, const FMasterItemDefinition&, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryBroadcastComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSctrollBoxResetSignature);
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPGSYSTEMS_API UInventoryWidgetController : public UWidgetController
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FInventoryItemSignature InventoryItemDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInventoryBroadcastComplete InventoryBroadcastCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
	FSctrollBoxResetSignature ScrollBoxResetDelegate;
	
	void SetOwningActor(AActor* InOwner);

	void BindCallbacksToDependencies();
	void BroadCastInitialValues();
	
private:

	void UpdateInventory(const FPackedInventory& InInventory);
	void BroadcastInventoryContent();
	
	UPROPERTY()
	TObjectPtr<AActor> OwningActor;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;
};
