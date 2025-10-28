// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/WidgetController.h"
#include "InventoryWidgetController.generated.h"

struct FMasterItemDefinition;
struct FPackedInventory;
class UInventoryComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemSignature, const FMasterItemDefinition&);
DECLARE_MULTICAST_DELEGATE(FOnInventoryBroadcastComplete);
DECLARE_MULTICAST_DELEGATE(FSctrollBoxResetSignature);
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPGSYSTEMS_API UInventoryWidgetController : public UWidgetController
{
	GENERATED_BODY()
public:

	
	FInventoryItemSignature InventoryItemDelegate;
	
	FOnInventoryBroadcastComplete InventoryBroadcastCompleteDelegate;
	
	FSctrollBoxResetSignature ScrollBoxResetDelegate;
	
	void SetOwningActor(AActor* InOwner);
	FORCEINLINE AActor* GetOwningActor() { return OwningActor; };

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
