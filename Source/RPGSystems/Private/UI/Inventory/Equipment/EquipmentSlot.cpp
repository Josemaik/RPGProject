// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Equipment/EquipmentSlot.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "InventorySection/InventoryComponent.h"
#include "Libraries/RPGUIStatics.h"
#include "UI/Inventory/ItemSlotDroppedDragDrop.h"
#include "UI/Inventory/ItemSlotIcon.h"
#include "UI/Inventory/ItemToolTip.h"

void UEquipmentSlot::NativeConstruct()
{
	Super::NativeConstruct();

	OnSlotMouseEnteredDelegate.BindUObject(this, &UEquipmentSlot::HandleMouseEntered);
	OnSlotMouseLeavedDelegate.BindUObject(this,  &UEquipmentSlot::HandleMouseLeaved);

	DragVisualEnable();

	//EmptySlot();
}

void UEquipmentSlot::EquipItemSlot(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& ItemDefinition)
{
	ItemEntry = Entry;
	EquipVisual(ItemDefinition.Icon.Get(), ItemDefinition.Rarity);
	DragOverPreview->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
	OnEquipItem.ExecuteIfBound(Entry);

	CurrentItemDefinition = ItemDefinition;
	CurrentSlotSize = ItemDefinition.SlotsSize > 1 ? SuperiorSlotVertical : LowerSlotVertical;
	bIsEmpty = false;

	SlotIcon = Cast<UItemSlotIcon>(CreateWidget(this,IconWidgetClass));
	if (IsValid(SlotIcon))
	{
		SlotIcon->SetIcon(CurrentItemDefinition.Icon.Get(),CurrentSlotSize);
	}
}

void UEquipmentSlot::DragVisualEnable()
{
	if (IsValid(IconBox) && IsValid(PlaceholderTexture))
	{
		IconBox->SetBrushFromTexture(PlaceholderTexture);
		IconBox->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	}
}

void UEquipmentSlot::EmptySlot()
{
	Super::EmptySlot();
	DragVisualEnable();
}

void UEquipmentSlot::HandleMouseEntered(UBaseInventorySlot* BaseSlot)
{
	if (!IsValid(ItemToolTipReference) || bIsEmpty) return;

	ItemToolTipReference->SetData(CurrentItemDefinition);

	FGeometry SlotGeometry = GetCachedGeometry();
	FVector2D AbsPos  = SlotGeometry.GetAbsolutePosition();
	FVector2D AbsSize = SlotGeometry.GetAbsoluteSize();

	FVector2D PixelPosition, ViewportPosition;
	USlateBlueprintLibrary::AbsoluteToViewport(
		GetWorld(),
		FVector2D(AbsPos.X + AbsSize.X, AbsPos.Y + AbsSize.Y),
		PixelPosition,
		ViewportPosition
	);

	ItemToolTipReference->SetPositionInViewport(ViewportPosition, false);
	ItemToolTipReference->SetVisibility(ESlateVisibility::Visible);
}

void UEquipmentSlot::HandleMouseLeaved(UBaseInventorySlot* BaseSlot)
{
	if (!IsValid(ItemToolTipReference)) return;
	ItemToolTipReference->SetVisibility(ESlateVisibility::Collapsed);
}

// void UEquipmentSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
// 	UDragDropOperation*& OutOperation)
// {
// 	if (bIsEmpty) return;
// 	
// 	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
// 	
// 	BuildDragOperation(OutOperation);
// }

FReply UEquipmentSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) return FReply::Unhandled();
	if (bIsEmpty) return FReply::Unhandled();

	ClickCount++;

	if (ClickCount >= 2)
	{
		ClickCount = 0;
		GetWorld()->GetTimerManager().ClearTimer(DoubleClickTimerHandle);

		FRPGInventoryEntry EntryCopy = ItemEntry;
		EmptySlot();
		OnUnequipItem.ExecuteIfBound(EntryCopy);
		return FReply::Handled();
	}
	
	GetWorld()->GetTimerManager().SetTimer(DoubleClickTimerHandle, [this]
	{
		ClickCount = 0;
	}, DoubleClickThreshold, false);
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UEquipmentSlot::BuildDragOperation(UDragDropOperation*& OutOperation)
{
	UItemSlotDroppedDragDrop* DragOp = Cast<UItemSlotDroppedDragDrop>(
	   UWidgetBlueprintLibrary::CreateDragDropOperation(UItemSlotDroppedDragDrop::StaticClass()));

	DragOp->Pivot             = EDragPivot::CenterCenter;
	DragOp->DefaultDragVisual = SlotIcon;
	DragOp->ItemEntry         = &ItemEntry;
	DragOp->SlotSize          = CurrentSlotSize; 
	DragOp->ItemDefinition = CurrentItemDefinition;
	DragOp->SourceEquipmentSlot = this;

	OutOperation = DragOp;
	
	FRPGInventoryEntry EntryCopy = ItemEntry;
	EmptySlot(); //-si cancelo prque no llego a dropear hay que restore
	//OnUnequipItem.ExecuteIfBound(EntryCopy);
}

bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
								  UDragDropOperation* InOperation)
{
	UItemSlotDroppedDragDrop* DragOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!DragOp || !DragOp->ItemEntry) return false;

	if (!DragOp->ItemEntry->ItemTag.MatchesTag(EquipmentTag)) return false;

	if (!bIsEmpty)
	{
		FRPGInventoryEntry OldEntry = ItemEntry;
		OnUnequipItem.ExecuteIfBound(OldEntry);
	}
	
	EquipItemSlot(*DragOp->ItemEntry,DragOp->ItemDefinition);
	SlotIcon = Cast<UItemSlotIcon>(DragOp->DefaultDragVisual);
	
	return true;
}

void UEquipmentSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	UItemSlotDroppedDragDrop* DragOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragOp) || !DragOp->ItemEntry) return;

	if (DragOp->SourceEquipmentSlot == this) return;
	
	const bool bCompatible = DragOp->ItemEntry->ItemTag.MatchesTag(EquipmentTag);
	if (IsValid(DragOverPreview))
	{
		DragOverPreview->SetColorAndOpacity(bCompatible
			? FLinearColor(0.287f, 0.138f, 0.041f, 0.5f)   
			: FLinearColor(1.f, 0.f, 0.f, 0.5f));  
	}
}

void UEquipmentSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (IsValid(DragOverPreview))
	{
		DragOverPreview->SetColorAndOpacity( FLinearColor(1.f, 1.f, 1.f, 0.f));
		BackgroundRarity->SetBrushTintColor(FLinearColor(0.f,0.f,0.f,0.f));
	}
}



