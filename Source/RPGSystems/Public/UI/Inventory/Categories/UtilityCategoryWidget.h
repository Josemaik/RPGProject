// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCategoryWidget.h"
#include "UtilityCategoryWidget.generated.h"
DECLARE_DELEGATE_TwoParams(FOnEquipentDropped,FGameplayTag ItemTag,uint64 ExistingID)
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UUtilityCategoryWidget : public UBaseCategoryWidget
{
	GENERATED_BODY()

public:
	FOnEquipentDropped OnEquipmentDropped;
	
	virtual void ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
									   const FMasterItemDefinition& Definition) override;
	virtual void RemoveEntry(const FRPGInventoryEntry& Entry) override;

	virtual const FRPGInventoryEntry* GetSelectedItem() override;

private:
	virtual void NativeConstruct() override;
	UPROPERTY(meta=(BindWidget))
	UGridItemsPanelWidget* QuestItemsPanel;   //filter tag: Item.Equipment.Weapon

	UPROPERTY(meta=(BindWidget))
	UGridItemsPanelWidget* OthersPanel;     //filter tag: Item.Equipment.Armor
};
