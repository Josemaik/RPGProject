// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/Inventory/Slots/SlotSizeCategories.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "GridItemsPanelWidget.generated.h"

class UBorder;
class USizeBox;
enum class EDragOverResult : uint8;
class UTextBlock;
class UItemSlotDragDrogOperation;
class UBaseInventorySlot;
class UItemToolTip;
enum class EItemSortType : uint8;
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
	int32 NumSlots; // 1 o 2
	
	UPROPERTY()
	bool bIsEmpty = true;

};


/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UGridItemsPanelWidget : public UUserWidget
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

	void BuildSortedItems(TArray<FItemSlotData>& SortedItems);
	void ResetWithSortedItems(TArray<FItemSlotData> SortedItems);
	void SortItemsQuicly();
	void SortItemsBy(EItemSortType SortType);

	const FRPGInventoryEntry* GetSelectedItem();

	FGameplayTag GetItemCategory(FGameplayTag ItemTag);
	int32 GetMaxColums() const { return MaxColumns; }
	UItemSlotWidget* GetItemSlotbyIndex(int32 Index) const;
	FGameplayTag GetSubCategoryTag() const { return SubCategoryTag; }
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

private:
	void HandleItemDropped(int32 DroppedIndex,int32 NewIndex,EDragOverResult SubCategoryResult);
	bool TryDropInNewSlot(int32 DroppedIndex,int32 NewIndex);
	bool TryDropFromEquipment(int32 ToIndex);
	void HandleDraggedItemEntered(int32 EnteredIndex,int32 NewIndex,EDragOverResult Result);
	void HandleDragEntered_FromEquipment(int32 TargetIndex, EDragOverResult SubCategoryResult);
	void HandleDragEntered_FromGrid(int32 DraggedIndex, int32 TargetIndex, EDragOverResult SubCategoryResult);
	void HandleDraggedItemLeaved(int32 DraggedIndex,int32 NewIndex,EDragOverResult Result);
	void HandleDragCancelled(int32 FailedIndex, int32 FromIndex, ESlotSizeCategories DraggedSize);
	
	void ClearDragPreviews(int32 SuperiorIndex, int32 LowerIndex,bool clearicon = true) const;
	void ApplyDragPreview(int32 SuperiorIndex, int32 LowerIndex, EDragOverResult Result);

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

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="UI", meta=(ExposeOnSpawn=true,AllowPrivateAccess=true))
	FText SubCategoryEditableText;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "UI")
	FGameplayTag SubCategoryTag;

	TArray<FItemSlotData> ItemsArray;
	int32 NumOcuppiedSlots = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 MaxColumns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 NUM_INITIAL_EMPTY_SLOTS = 35;
	
	//References
	UPROPERTY()
	TObjectPtr<UItemToolTip> ItemToolTipReference;

	UPROPERTY()
	TObjectPtr<UItemSlotDragDrogOperation> CurrentDragOperation;
	
	//Subclass
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UItemToolTip> TooltipWidgetClass;
	
	//////////////////////
	// Layout
	
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "UI")
	TObjectPtr<USizeBox> SizeBox;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	TObjectPtr<UUniformGridPanel> ItemsPanel;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	TObjectPtr<UTextBlock> SubCategoryText;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "UI")
	TObjectPtr<UBorder> BorderWidget;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SelectedPanelAnimation;
};



