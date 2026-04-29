// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCategoryWidget.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "AlchemyCategoryConsumablesWidget.generated.h"

class UItemsPanelWidget;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UAlchemyCategoryConsumablesWidget : public UBaseCategoryWidget
{
	GENERATED_BODY()

public:
	virtual void ReceiveInventoryEntry(const FRPGInventoryEntry& Entry,
									   const FMasterItemDefinition& Definition) override;
	virtual void RemoveEntry(int64 ItemID) override;
private:
	UPROPERTY(meta=(BindWidget))
	UItemsPanelWidget* OilsPanel;  

	UPROPERTY(meta=(BindWidget))
	UItemsPanelWidget* BombsPanel;    

	UPROPERTY(meta=(BindWidget))
	UItemsPanelWidget* PotionsPanel; 

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag OilsTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag BombsTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PotionsTag;
};
