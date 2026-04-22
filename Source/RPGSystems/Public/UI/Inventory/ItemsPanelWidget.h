// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "ItemsPanelWidget.generated.h"

enum class EItemSortType : uint8;
class USortPanelWidget;
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
	FMasterItemDefinition ItemDefinition;

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY()
	FSlateBrush IconBrush;
	
	TEnumAsByte<ESlotSizeCategories>  Size;
	
	UPROPERTY()
	bool bIsEmpty = true;
};

struct FLogicalItem
{
	FRPGInventoryEntry Entry;
	FMasterItemDefinition ItemDefinition;
	int32 Size; // 1 o 2
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
	void ProcessSlotWidget(const FItemSlotData& SlotData, UItemSlotWidget* NewWidget, FSlateBrush Brush);
	void AddItemSlot(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& ItemDefinition);
	void AddEmptySlots(FGameplayTag InCurrentCategoryTag);
	void CreateSlotWidget(int32 Index,const FItemSlotData& SlotData);
	void ClearPanel();
	
	void ResetCategory(FGameplayTag CurrentCategoryTag);

	void SortItemsQuicly();
	void SortItemsBy(EItemSortType SortType);

	FGameplayTag GetItemCategory(FGameplayTag ItemTag);
	int32 GetMaxColums() const { return MaxColumns; }

	void HandleItemDropped(UItemSlotWidget* DroppedSlot,UItemSlotWidget* NewSlot);
	void HandleDraggedItemEntered(int32 EnteredIndex,int32 NewIndex);
	void HandleDraggedItemLeaved(int32 DraggedIndex,int32 NewIndex);
	
	FGameplayTag CurrentCategoryTag;

private:
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UUniformGridPanel* ItemsPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;
	
	TMap<FGameplayTag, TArray<FItemSlotData>> CategoryItemsMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 MaxColumns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 NUM_INITIAL_EMPTY_SLOTS = 35;
};


