// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "ItemSlotWidget.generated.h"

class UImage;
class UUniformGridSlot;
class UInventoryWidgetController;
class USizeBox;
class UItemSlotIcon;
struct FRPGInventoryEntry;
struct FMasterItemDefinition;
class UTextBlock;
class UButton;



UENUM()
enum ESlotSizeCategories : uint8
{
	UniqueSlot,
	SuperiorSlotVertical,
	LowerSlotVertical
};

class UItemSlotWidget;

DECLARE_DELEGATE_OneParam(FOnItemRowClicked, const FRPGInventoryEntry&);
DECLARE_DELEGATE_OneParam(FOnItemDroppedEvent, const FRPGInventoryEntry&);
DECLARE_DELEGATE_TwoParams(FOnItemDroppedPanel, UItemSlotWidget* OldSlot, UItemSlotWidget* NewSlot);
DECLARE_DELEGATE_OneParam(FOnDragEntered, int32 NewIndex);
DECLARE_DELEGATE_OneParam(FOnDragLeaved, int32 NewIndex);
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnItemRowClicked OnItemRowClickedDelegate;
	FOnItemDroppedEvent OnItemDroppedEventDelegate;
	FOnItemDroppedPanel OnItemDroppedPanelDelegate;
	FOnDragEntered OnDragEnteredDelegate;
	FOnDragLeaved OnDragLeavedDelegate;
	
	void SetItemNameText(FText Text);
	void SetQuantityText(int32 Quantity);
	void SetIcon(const FSlateBrush& Brush);
	void SetGridSlot(UUniformGridSlot* GridSlot);
	UUniformGridSlot* GetGridSlot();
	
	void Init(const FRPGInventoryEntry& Entry,const TSoftObjectPtr<UTexture2D>& Icon,const FSlateBrush& Brush = FSlateBrush(), ESlotSizeCategories SlotSize = ESlotSizeCategories::UniqueSlot);
	void EmptySlot();
	void OutlineSlot(ESlotSizeCategories SlotSize);
	void RemoveOutLineSlot(bool OnDrop);
	void SetIconPadding(bool reset) const;
	bool IsEmpty() const { return bIsEmpty; }
	TSoftObjectPtr<UTexture2D> GetIconTexture() const { return  SoftIconTexture; }
	int32 GetGridIndex() const { return CurrentGridIndex; }
	void SetGridIndex(const int32 NewIndex) { CurrentGridIndex = NewIndex; }
	ESlotSizeCategories GetCurrentSlotSize() const { return CurrentSlotSize; }
	const FSlateBrush& GetIconBrush() const { return CurrentIconBrush; }
	
	UPROPERTY(BlueprintReadOnly)
	FRPGInventoryEntry ItemEntry;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
private:
	ESlotSizeCategories CurrentSlotSize;
	
	bool bIsEmpty = true;
	
	int32 CurrentGridIndex = 0;
	
	FSlateBrush CurrentIconBrush;

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> SoftIconTexture;
	
	UPROPERTY()
	TObjectPtr<UUniformGridSlot> GridSlot = nullptr;
	
	UPROPERTY()
	UItemSlotIcon* IconWidgetReference;
	
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<UItemSlotIcon> IconWidgetClass;
	
	//Hierarchy
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidgetOptional,AllowPrivateAccess="true"), Category = "UI")
	UTextBlock* ItemQuantity;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UImage* IconBox;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UImage* Border;
};



