// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCategoryWidget.h"
#include "AlchemyCategoryConsumablesWidget.generated.h"

class UGridItemsPanelWidget;
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
	virtual void RemoveEntry(const FRPGInventoryEntry& Entry) override;
private:

	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	UGridItemsPanelWidget* OilsPanel;  

	UPROPERTY(meta=(BindWidget))
	UGridItemsPanelWidget* BombsPanel;    

	UPROPERTY(meta=(BindWidget))
	UGridItemsPanelWidget* PotionsPanel; 
};