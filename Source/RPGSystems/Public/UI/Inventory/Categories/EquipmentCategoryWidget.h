// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCategoryWidget.h"
#include "EquipmentCategoryWidget.generated.h"

DECLARE_DELEGATE_TwoParams(FOnEquipentDropped,FGameplayTag ItemTag,uint64 ExistingID)

class UItemsPanelWidget;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UEquipmentCategoryWidget : public UBaseCategoryWidget
{
	GENERATED_BODY()

public:
	FOnEquipentDropped OnEquipmentDropped;
	
	virtual void ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
									   const FMasterItemDefinition& Definition) override;
	virtual void RemoveEntry(int64 ItemID) override;

	virtual const FRPGInventoryEntry* GetSelectedItem() override;

private:
	virtual void NativeConstruct() override;
	UPROPERTY(meta=(BindWidget))
	UItemsPanelWidget* WeaponsPanel;   //filter tag: Item.Equipment.Weapon

	UPROPERTY(meta=(BindWidget))
	UItemsPanelWidget* ArmorPanel;     //filter tag: Item.Equipment.Armor
};
