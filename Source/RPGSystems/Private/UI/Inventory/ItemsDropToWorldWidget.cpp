// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemsDropToWorldWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/Inventory/Slots/ItemSlotDragDrogOperation.h"

void UItemsDropToWorldWidget::NativeDestruct()
{
	Super::NativeDestruct();
	OnItemDroppedPanelDelegate.Unbind();
}

bool UItemsDropToWorldWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                           UDragDropOperation* InOperation)
{
	UItemSlotDragDrogOperation* DragDropOp = Cast<UItemSlotDragDrogOperation>(InOperation);
	if (!IsValid(DragDropOp)) return false;
	
	OnItemDroppedPanelDelegate.ExecuteIfBound(*DragDropOp->ItemEntry);

	SetDropHighlight(false);
	
	return true;
}


void UItemsDropToWorldWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	SetDropHighlight(false);
}

void UItemsDropToWorldWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	SetDropHighlight(true);
}

void UItemsDropToWorldWidget::SetDropHighlight(bool bEnabled) const
{
	if (!IsValid(DropItemImage) || !IsValid(DropItemText)) return;
	
	const FLinearColor& Color = DropItemImage->GetColorAndOpacity();
	const float Intensity = bEnabled ? 1.0f : 0.2f;
	DropItemImage->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, Intensity));
	DropItemText->SetColorAndOpacity(FLinearColor(Color.R, Color.G, Color.B, Intensity));
}
