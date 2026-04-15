// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "ItemsPanelWidget.generated.h"

enum ESlotSizeCategories : uint8;
struct FMasterItemDefinition;
struct FRPGInventoryEntry;
class UItemSlotWidget;
class UUniformGridPanel;


/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemsPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddItemToGrid(UItemSlotWidget* Item,const int32 Index);
	void RemoveItem(const int64 ItemID);
	UItemSlotWidget* ContainsItemSlot(const int64 ItemID);
	
	void UpdateItemSlot(const FRPGInventoryEntry& Entry);
	FGameplayTag GetItemCategory(FGameplayTag ItemTag);
	UItemSlotWidget* AddItemSlot(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& ItemDefinition,int32 Index = INDEX_NONE);
	
	void ClearPanel();
	int32 GetMaxColums() const { return MaxColumns; }
	void AddEmptySlots(FGameplayTag InCurrentCategoryTag);
	void ResetCategory(FGameplayTag CurrentCategoryTag);

	void HandleItemDropped(UItemSlotWidget* DroppedSlot,UItemSlotWidget* NewSlot);
	void HandleDraggedItemEntered(int32 NewIndex);
	void HandleDraggedItemLeaved(int32 NewIndex);
	
	FGameplayTag CurrentCategoryTag;

private:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UUniformGridPanel* ItemsPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;
	
	//TMap<FGameplayTag, int32> CategoriesIndexMap;
	TMap<FGameplayTag, TArray<UItemSlotWidget*>> CategoryItemsMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 MaxColumns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 NUM_INITIAL_EMPTY_SLOTS = 35;
};


