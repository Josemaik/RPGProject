// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemDragVisualWidget.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "UI/Inventory/SlotSizeCategories.h"

void UItemDragVisualWidget::SetIcon(TObjectPtr<UTexture2D> IconTexture, ESlotSizeCategories Size)
{
	if (!IsValid(SizeBox) || !IsValid(DragIcon)) return;
	
	FVector2f IconSize = (Size == LowerSlotVertical || Size == SuperiorSlotVertical) ?
		IconSizeTwoSlots : IconSizeUniqueSlot;
	
	SizeBox->SetWidthOverride(IconSize.X);
	SizeBox->SetHeightOverride(IconSize.Y);
	DragIcon->SetBrushFromTexture(IconTexture);
}

void UItemDragVisualWidget::EnableDragOverResultIcon(EDragOverResult Result)
{
	if (!IsValid(DragOverResultIcon)) return;
	DragOverResultIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	switch (Result)
	{
	case EDragOverResult::Drop:
		DragOverResultIcon->SetBrushFromTexture(DragOverResultDropTexture.Get());
		break;
	case EDragOverResult::Swap:
		DragOverResultIcon->SetBrushFromTexture(DragOverResultSwapTexture.Get());
		break;
	case EDragOverResult::Invalid:
		DragOverResultIcon->SetBrushFromTexture(DragOverResultInvalidTexture.Get());
		break;
	}
}

void UItemDragVisualWidget::DisableDragOverResultIcon()
{
	if (!IsValid(DragOverResultIcon)) return;
	DragOverResultIcon->SetVisibility(ESlateVisibility::Collapsed);
}
