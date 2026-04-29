// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCategoryWidget.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentCategoryWidget.generated.h"

class UItemsPanelWidget;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UEquipmentCategoryWidget : public UBaseCategoryWidget
{
	GENERATED_BODY()

public:
	virtual void ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
									   const FMasterItemDefinition& Definition) override;
	virtual void RemoveEntry(int64 ItemID) override;

	virtual const FRPGInventoryEntry& GetSelectedItem();
private:
	UPROPERTY(meta=(BindWidget))
	UItemsPanelWidget* WeaponsPanel;   //filter tag: Item.Equipment.Weapon

	UPROPERTY(meta=(BindWidget))
	UItemsPanelWidget* ArmorPanel;     //filter tag: Item.Equipment.Armor

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WeaponsTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ArmorTag;
};
