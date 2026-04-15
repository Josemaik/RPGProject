// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "ItemsPanelWidget.generated.h"

enum ESlotSizeCategories : uint8;
struct FMasterItemDefinition;
struct FRPGInventoryEntry;
class UItemSlotWidget;
class UUniformGridPanel;

USTRUCT()
struct FItemSlotData
{
	GENERATED_BODY()

	UPROPERTY()
	FRPGInventoryEntry Entry;

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY()
	FSlateBrush IconBrush;
	
	TEnumAsByte<ESlotSizeCategories>  Size;
	
	UPROPERTY()
	bool bIsEmpty = true;
};

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
	int32 FindItemIndex(const int64 ItemID,FGameplayTag ItemTa);
	
	void UpdateItemSlot(const FRPGInventoryEntry& Entry);
	FGameplayTag GetItemCategory(FGameplayTag ItemTag);
	void AddItemSlot(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& ItemDefinition);
	
	void ClearPanel();
	int32 GetMaxColums() const { return MaxColumns; }
	void AddEmptySlots(FGameplayTag InCurrentCategoryTag);
	void OnIconLoaded(UItemSlotWidget* ItemSlotWidget, FItemSlotData ItemSlotData);
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
	//TMap<FGameplayTag, TArray<UItemSlotWidget*>> CategoryItemsMap;
	TMap<FGameplayTag, TArray<FItemSlotData>> CategoryItemsMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 MaxColumns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 NUM_INITIAL_EMPTY_SLOTS = 35;
};


