// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseCategoryWidget.generated.h"

struct FMasterItemDefinition;
struct FRPGInventoryEntry;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UBaseCategoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void ReceiveInventoryEntry(const FRPGInventoryEntry& Entry, 
									   const FMasterItemDefinition& Definition) {}
	virtual void RemoveEntry(int64 ItemID) {}
	virtual void UpdateEntry(const FRPGInventoryEntry& Entry) {}
	virtual const FRPGInventoryEntry& GetSelectedItem(){ return *LastSelectedEntry; };
protected:
	FRPGInventoryEntry* LastSelectedEntry;
};
