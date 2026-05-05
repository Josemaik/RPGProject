// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SlotSizeCategories.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "ItemsPanelWidget.generated.h"

class UBorder;
class USizeBox;
enum class EDragOverResult : uint8;
class UTextBlock;
class UItemSlotDragDrogOperation;
class UBaseInventorySlot;
class UItemToolTip;
enum class EItemSortType : uint8;
class USortPanelWidget;
enum class ESlotSizeCategories : uint8;
struct FMasterItemDefinition;
struct FRPGInventoryEntry;
class UItemSlotWidget;
class UUniformGridPanel;

DECLARE_DELEGATE_TwoParams(FOnEquipentDropped,FGameplayTag ItemTag,uint64 ExistingID)
DECLARE_DELEGATE_OneParam(FOnSelectItem, FGameplayTag SubCategory)
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
	
	ESlotSizeCategories Size;
	
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
	FOnEquipentDropped OnEquipmentDropped;
	FOnSelectItem OnSelectItemDelegate;
	
	void AddItemToGrid(UItemSlotWidget* Item,const int32 Index);
	void RemoveItem(const FRPGInventoryEntry& Entry);
	int32 FindGridIndexByItemID(const int64 ItemID,FGameplayTag ItemTag);
	
	void UpdateItemSlot(const FRPGInventoryEntry& Entry);
	void InitializeSlotWidget(const FItemSlotData& SlotData, UItemSlotWidget* NewWidget, FSlateBrush Brush);
	void BindItemSlotDelegates(UItemSlotWidget* NewWidget);
	void AddItemSlot(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& ItemDefinition,bool bResetPanel = true);
	void AddEmptySlots();
	void CreateSlotWidget(int32 Index,const FItemSlotData& SlotData);
	void ClearPanel();

	void ActivateCurrentSelectedSlot();
	void DeactivateCurrentSelectedSlot();
	
	void ResetItemsArray();

	void SortItemsQuicly();
	void SortItemsBy(EItemSortType SortType);

	const FRPGInventoryEntry* GetSelectedItem();

	FGameplayTag GetItemCategory(FGameplayTag ItemTag);
	int32 GetMaxColums() const { return MaxColumns; }
	
	UItemSlotWidget* GetItemSlotbyIndex(int32 Index) const;
	FGameplayTag GetSubCategoryTag() const { return SubCategoryTag; }
	
	//FGameplayTag CurrentCategoryTag;
	
	UPROPERTY()
	UItemSlotDragDrogOperation* CurrentDragOperation;
	

private:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	
	void HandleItemDropped(int32 DroppedIndex,int32 NewIndex,EDragOverResult SubCategoryResult);
	bool TryDropInNewSlot(int32 DroppedIndex,int32 NewIndex);
	void HandleDraggedItemEntered(int32 EnteredIndex,int32 NewIndex,EDragOverResult Result);
	void HandleDraggedItemLeaved(int32 DraggedIndex,int32 NewIndex,EDragOverResult Result);
	void HandleDragCancelled(int32 FailedIndex, int32 FromIndex, ESlotSizeCategories DraggedSize);

	void HandleSlotHovered(UBaseInventorySlot* BaseSlot);
	void HandleSlotLeaved(UBaseInventorySlot* SlotWidget);
	
	void HandleSlotClicked(int32 ClickedIndex);
	void SelectCurrentIndexSlot(int32 NewIndex);
	void DeactivateSlotAtIndex(int32 Index);
	void ActivateSlotAtIndex(int32 Index);

	void HandleEquipmentEntered(UItemSlotDragDrogOperation* InCurrentDragOperation);
	
	void ResetSlotData(FItemSlotData& ItemSlotData);
	int32 LastHoveredIndex = INDEX_NONE;
	int32 OldSelectedIndex = INDEX_NONE;
	int32 CurrentSelectedIndex = 0;
	bool IsPanelFocus = false;

	UPROPERTY(EditAnywhere, Category = "Grid")
	float SlotSize = 90.f;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "UI")
	USizeBox* SizeBox;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UUniformGridPanel* ItemsPanel;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UTextBlock* SubCategoryText;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="UI", meta=(ExposeOnSpawn=true,AllowPrivateAccess=true))
	FText SubCategoryEditableText;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "UI")
	FGameplayTag SubCategoryTag;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "UI")
	UBorder* BorderWidget;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* SelectedPanelAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

	UPROPERTY()
	UItemToolTip* ItemToolTipReference;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UItemToolTip> TooltipWidgetClass;

	TArray<FItemSlotData> ItemsArray;
	int32 NumOcuppiedSlots = 0;
	//TMap<FGameplayTag, TArray<FItemSlotData>> CategoryItemsMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 MaxColumns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 NUM_INITIAL_EMPTY_SLOTS = 35;
};


