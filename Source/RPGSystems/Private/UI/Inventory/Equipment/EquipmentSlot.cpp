// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Equipment/EquipmentSlot.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "InventorySection/InventoryComponent.h"
#include "Libraries/RPGUIStatics.h"
#include "UI/Inventory/ItemSlotDragDrogOperation.h"
#include "UI/Inventory/ItemDragVisualWidget.h"
#include "UI/Inventory/ItemToolTip.h"

void UEquipmentSlot::NativeConstruct()
{
	Super::NativeConstruct();

	OnSlotMouseEnteredDelegate.BindUObject(this, &UEquipmentSlot::HandleMouseEntered);
	OnSlotMouseLeavedDelegate.BindUObject(this,  &UEquipmentSlot::HandleMouseLeaved);

	IconBox->SetBrushFromTexture(PlaceholderTexture);
	//DragVisualEnable();

	//EmptySlot();
}

void UEquipmentSlot::EquipItemSlot(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& ItemDefinition)
{
	ItemEntry = Entry;
	EquipVisual(ItemDefinition.Icon.Get(), ItemDefinition.RarityTag);
	
	IconBoxSlot = Cast<UOverlaySlot>(IconBox->Slot);
	if (!IsValid(IconBoxSlot)) return;

	IconBoxSlot->SetPadding(0.f);
	
	DragOverPreview->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
	
	OnEquipItem.ExecuteIfBound(Entry);

	CurrentItemDefinition = ItemDefinition;
	CurrentSlotSize = ItemDefinition.SlotsSize > 1 ? ESlotSizeCategories::SuperiorSlotVertical : ESlotSizeCategories::UniqueSlot;
	bIsEmpty = false;
}

void UEquipmentSlot::DragVisualEnable(bool bEnable)
{
	if (IsValid(IconBox) && IsValid(PlaceholderTexture))
	{
		//IconBox->SetBrushFromTexture(PlaceholderTexture);
		//FLinearColor Color = bEnable ? FLinearColor(1.f, 1.f, 1.f, 1.f) : FLinearColor(0.f, 0.f, 0.f, 1.f);
		//IconBox->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	}
	if (IsValid(BackgroundRarity))
	{
		BackgroundRarity->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
	}
}

void UEquipmentSlot::DisableDragOverPreview()
{
	DragOverPreview->SetColorAndOpacity( FLinearColor(1.f, 1.f, 1.f, 0.f));
}

void UEquipmentSlot::NativePreConstruct()
{
	Super::NativePreConstruct();
	SizeBox->SetWidthOverride(SlotWidth);
	SizeBox->SetHeightOverride(SlotHeight);
}

void UEquipmentSlot::EmptySlot()
{
	Super::EmptySlot();
	IconBox->SetBrushFromTexture(PlaceholderTexture);
	//	IconBoxSlot->SetPadding(20.f);
	IconBox->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	//DragVisualEnable();
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

FReply UEquipmentSlot::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsEmpty) return FReply::Unhandled();
	if (!IsValid(ItemToolTipReference)) return FReply::Unhandled();
	
	FRPGInventoryEntry EntryCopy = ItemEntry;
	EmptySlot();
	OnUnequipItem.ExecuteIfBound(EntryCopy);
	OnEquipmentDropped.ExecuteIfBound(EntryCopy.ItemTag,EntryCopy.ItemID);

	BackgroundRarity->SetBrushTintColor(FLinearColor(0.f,0.f,0.f,0.f));
	
	ItemToolTipReference->SetVisibility(ESlateVisibility::Collapsed);
	return FReply::Handled();
}


void UEquipmentSlot::BuildDragOperation(UDragDropOperation*& OutOperation)
{
	UItemSlotDragDrogOperation* DragOp = Cast<UItemSlotDragDrogOperation>(
	   UWidgetBlueprintLibrary::CreateDragDropOperation(UItemSlotDragDrogOperation::StaticClass()));

	UItemDragVisualWidget* DragVisual = CreateWidget<UItemDragVisualWidget>(GetWorld(),IconWidgetClass);
	if (!IsValid(DragVisual)) return;
	
	DragVisual->SetIcon(CurrentItemDefinition.Icon.Get(),CurrentSlotSize);
	
	DragOp->Pivot             = EDragPivot::CenterCenter;
	DragOp->ItemDraggedIconWidget = DragVisual;
	DragOp->DefaultDragVisual = DragVisual;
	
	DragOp->ItemEntry         = &ItemEntry;
	DragOp->SlotSize          = CurrentSlotSize; 
	DragOp->ItemDefinition = CurrentItemDefinition;
	DragOp->SourceEquipmentSlot = this;

	OutOperation = DragOp;
	
	FRPGInventoryEntry EntryCopy = ItemEntry;

	IconBox->SetBrushFromTexture(PlaceholderTexture);
	BackgroundRarity->SetOpacity(0.f);
}

bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
								  UDragDropOperation* InOperation)
{
	UItemSlotDragDrogOperation* DragOp = Cast<UItemSlotDragDrogOperation>(InOperation);
	if (!DragOp || !DragOp->ItemEntry) return false;

	if (!DragOp->ItemDefinition.SlotTag.MatchesTag(SlotTag)) return false;

	if (!bIsEmpty)
	{
		FRPGInventoryEntry OldEntry = ItemEntry;
		OnUnequipItem.ExecuteIfBound(OldEntry);
	}
	
	EquipItemSlot(*DragOp->ItemEntry,DragOp->ItemDefinition);
	
	DragOp->ItemDraggedIconWidget->DisableDragOverResultIcon();
	
	return true;
}

void UEquipmentSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	UItemSlotDragDrogOperation* DragOp = Cast<UItemSlotDragDrogOperation>(InOperation);
	if (!IsValid(DragOp) || !DragOp->ItemEntry) return;

	//if (DragOp->SourceEquipmentSlot == this) return;
	DragOp->LastEnterSlotWidget = this;
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,FString::Printf(TEXT("End Drag Equip")));
	
	const bool bCompatible = DragOp->ItemDefinition.SlotTag.MatchesTag(SlotTag);
	if (IsValid(DragOverPreview))
	{
		DragOverPreview->SetColorAndOpacity(bCompatible
			? FLinearColor(0.287f, 0.138f, 0.041f, 0.5f)   
			: FLinearColor(1.f, 0.f, 0.f, 0.5f));
	}
	
	EDragOverResult Result = bCompatible ? EDragOverResult::Drop : EDragOverResult::Invalid;
	DragOp->ItemDraggedIconWidget->EnableDragOverResultIcon(Result);
}

void UEquipmentSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (IsValid(DragOverPreview))
	{
		DragOverPreview->SetColorAndOpacity( FLinearColor(1.f, 1.f, 1.f, 0.f));
		BackgroundRarity->SetBrushTintColor(FLinearColor(0.f,0.f,0.f,0.f));
	}
	UItemSlotDragDrogOperation* DragOp = Cast<UItemSlotDragDrogOperation>(InOperation);
	if (!IsValid(DragOp)) return;
	DragOp->ItemDraggedIconWidget->DisableDragOverResultIcon();
	
	bIsEmpty = true;
}




