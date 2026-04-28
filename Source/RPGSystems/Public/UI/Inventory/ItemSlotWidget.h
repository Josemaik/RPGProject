// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemSlotDragDrogOperation.h"
#include "SlotSizeCategories.h"
#include "Base/BaseInventorySlot.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/URPGDragDropInterface.h"
#include "InventorySection/InventoryComponent.h"
#include "ItemSlotWidget.generated.h"

enum class EDragOverResult : uint8;
class UItemToolTip;
class UImage;
class UUniformGridSlot;
class UInventoryWidgetController;
class USizeBox;
class UItemDragVisualWidget;
struct FRPGInventoryEntry;
class UTextBlock;
class UButton;

class UItemSlotWidget;

DECLARE_DELEGATE_OneParam(FOnItemRowClicked, int32 ClickedItemIndex);
DECLARE_DELEGATE_TwoParams(FOnItemDroppedPanel, int32 DroppedItemIndex, int32 DropItemIndex);
DECLARE_DELEGATE_TwoParams(FOnDragEntered,int32 DraggedItemIndex, int32 EnterItemIndex);
DECLARE_DELEGATE_TwoParams(FOnDragLeaved, int32 DraggedItemIndex,int32 LeaveItemIndex);
DECLARE_DELEGATE_OneParam(FOnDragCancelled,const FRPGInventoryEntry& Entry);
DECLARE_DELEGATE_OneParam(FOnNewDragOperation,UItemSlotDragDrogOperation* DragDropOp);
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemSlotWidget : public UBaseInventorySlot
{
	GENERATED_BODY()
public:
	FOnItemRowClicked OnItemRowClickedDelegate;
	FOnItemDroppedPanel OnItemDroppedPanelDelegate;
	FOnDragEntered OnDragEnteredDelegate;
	FOnDragLeaved OnDragLeavedDelegate;
	FOnDragCancelled OnDragCancelledDelegate;
	FOnNewDragOperation OnNewDragOperation;
	//FOnItemSlotMouseEntered OnItemSlotMouseEnteredDelegate;
	//FOnItemSlotMouseLeaved OnItemSlotMouseLeavedDelegate;

	void SetItemNameText(FText Text);
	void SetQuantityText(int32 Quantity);
	void SetIcon(const FSlateBrush& Brush);
	void SetGridSlot(UUniformGridSlot* GridSlot);
	UUniformGridSlot* GetGridSlot();
	
	void Init(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& Definition,FSlateBrush Brush = FSlateBrush(), ESlotSizeCategories SlotSize = ESlotSizeCategories::UniqueSlot);
	void EmptySlot() override;
	void OutlineSlot() const;
	void RemoveOutLineSlot();
	void SetIconPadding(bool reset) const;
	
	void EnableDragOverPreview(EDragOverResult Result);
	virtual void DisableDragOverPreview() override;
	//void EnableDragOverResultIcon(EDragOverResult Result,ESlotSizeCategories DraggedSize);
	//void DisableDragOverResultIcon();

	void StartSelectedAnimation();
	void StopSelectedAnimation();
	
	//void SetLinkedSlot(UItemSlotWidget* InSlot) { LinkedSlot = InSlot; }
	
	//bool IsEmpty() const { return bIsEmpty; }
	//TSoftObjectPtr<UTexture2D> GetIconTexture() const { return  SoftIconTexture; }
	int32 GetGridIndex() const { return CurrentGridIndex; }
	void SetGridIndex(const int32 NewIndex) { CurrentGridIndex = NewIndex; }
	//ESlotSizeCategories GetCurrentSlotSize() const { return CurrentSlotSize; }
	//const FSlateBrush& GetIconBrush() const { return CurrentIconBrush; }
	
	// UPROPERTY(BlueprintReadOnly)
	// FRPGInventoryEntry ItemEntry;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void BuildDragOperation(UDragDropOperation*& OutOperation) override; 
	//virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
private:
	// virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	// virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	//void CancelLeaveTimer();

	// UPROPERTY()
	// TObjectPtr<UItemSlotWidget> LinkedSlot;

	// UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	// UImage* DragOverResultIcon;

	//FTimerHandle ItemTooltipTimerHandle;
	//FTimerHandle ItemTooltipLeaveTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category="Style")
	FLinearColor DragOverPreviewColor;
	
	//ESlotSizeCategories CurrentSlotSize = ESlotSizeCategories::UniqueSlot;
	//bool bIsEmpty = true;
	int32 CurrentGridIndex = 0;
	//FSlateBrush CurrentIconBrush;
	//EItemRarity CurrentRarity;
	int32 MaxColumns = 5;

	// UPROPERTY()
	// TSoftObjectPtr<UTexture2D> SoftIconTexture;
	
	UPROPERTY()
	TObjectPtr<UUniformGridSlot> GridSlot = nullptr;
	
	// UPROPERTY()
	// UItemSlotIcon* IconWidgetReference;
	
	// UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	// TSubclassOf<UItemSlotIcon> IconWidgetClass;

	UPROPERTY()
	UItemToolTip* TooltipWidgetReference;
	
	//Hierarchy
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidgetOptional,AllowPrivateAccess="true"), Category = "UI")
	UTextBlock* ItemQuantity;
	
	// UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	// UImage* IconBox;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UImage* DragOverPreview;

	// UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	// UImage* BackgroundRarity;

	// UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	// UImage* DragOverResultIcon;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UImage* Border;

	//Animations
	// UPROPERTY(Transient, meta=(BindWidgetAnim))
	// UWidgetAnimation* SelectedSlotAnimation;
};



