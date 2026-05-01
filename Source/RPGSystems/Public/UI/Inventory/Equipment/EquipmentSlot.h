// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/URPGDragDropInterface.h"
#include "UI/Inventory/Base/BaseInventorySlot.h"
#include "EquipmentSlot.generated.h"

class UOverlaySlot;
enum class EItemRarity : uint8;
class USizeBox;
struct FRPGInventoryEntry;
class UImage;

DECLARE_DELEGATE_OneParam(FOnEquipItem, const FRPGInventoryEntry&)
DECLARE_DELEGATE_OneParam(FOnUnequipItem, const FRPGInventoryEntry&)
DECLARE_DELEGATE_TwoParams(FOnEquipentDropped,FGameplayTag ItemTag,uint64 ExistingID)
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UEquipmentSlot : public UBaseInventorySlot
{
	GENERATED_BODY()
public:
	void EquipItemSlot(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& ItemDefinition);
	void SetTooltipReference(UItemToolTip* InTooltip) { ItemToolTipReference = InTooltip; }
	virtual void EmptySlot() override;
	FGameplayTag GetSlotTag() const { return SlotTag; };
	
	FOnEquipItem OnEquipItem;
	FOnUnequipItem OnUnequipItem;
	FOnEquipentDropped OnEquipmentDropped;
protected:
	virtual void NativeConstruct() override;
	//virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void BuildDragOperation(UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void DragVisualEnable(bool bEnable) override;
	virtual void DisableDragOverPreview() override;
private:
	virtual void NativePreConstruct() override;
	void HandleMouseEntered(UBaseInventorySlot* BaseSlot);
	void HandleMouseLeaved(UBaseInventorySlot* BaseSlot);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Config",meta=(AllowPrivateAccess=true))
	FGameplayTag SlotTag;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess=true))
	UTexture2D* PlaceholderTexture;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(BindWidget,AllowPrivateAccess=true))
	UImage* DragOverPreview;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Config",meta=(AllowPrivateAccess=true))
	float SlotWidth;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Config",meta=(AllowPrivateAccess=true))
	float SlotHeight;
	
	UPROPERTY()
	UItemToolTip* ItemToolTipReference;

	UPROPERTY()
	UOverlaySlot* IconBoxSlot;
	
	int32 ClickCount = 0;
	FTimerHandle DoubleClickTimerHandle;
	static constexpr float DoubleClickThreshold = 1.f;
};
