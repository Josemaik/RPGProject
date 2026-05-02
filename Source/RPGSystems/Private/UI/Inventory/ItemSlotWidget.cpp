// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemSlotWidget.h"

#include "GameplayTagContainer.h"
#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemSlotDragDrogOperation.h"
#include "Components/OverlaySlot.h"
#include "Libraries/RPGUIStatics.h"
#include "UI/Inventory/ItemDragVisualWidget.h"


void UItemSlotWidget::SetItemNameText(FText Text)
{
	//ItemName->SetText(Text);
}

void UItemSlotWidget::SetQuantityText(int32 Quantity)
{
	if (!IsValid(this)) return;
	
	if (!ItemEntry.ItemTag.IsValid() || ItemEntry.ItemTag.MatchesTag(RPGInventoryTags::ItemsCategory::Equipment)) return;

	if (!ItemQuantity) return;
	
	const FText FormatText = FText::FromString(FString("x") + FString::FromInt(Quantity));
	ItemQuantity->SetText(FormatText);
}

void UItemSlotWidget::SetIcon(const FSlateBrush& Brush)
{
	if (!IsValid(IconBox)) return;
	
	IconBox->SetBrush(Brush);
	IconBox->SetColorAndOpacity(FLinearColor::White);
}

void UItemSlotWidget::SetGridSlot(UUniformGridSlot* NewGridSlot)
{
	GridSlot = NewGridSlot;
}

UUniformGridSlot* UItemSlotWidget::GetGridSlot()
{
	return GridSlot;
}

void UItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UItemSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	OnItemRowClickedDelegate.Unbind();
	OnItemDroppedPanelDelegate.Unbind();
	OnDragEnteredDelegate.Unbind();
	OnDragLeavedDelegate.Unbind();

	Super::NativeDestruct();
}

void UItemSlotWidget::Init(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& Definition,FSlateBrush Brush, ESlotSizeCategories SlotSize)
{
	ItemEntry = Entry;
	CurrentItemDefinition = Definition;
	SetQuantityText(ItemEntry.Quantity);
	
	bIsEmpty = false;
	CurrentSlotSize = SlotSize;

	CurrentIconBrush = Brush;
	SetIcon(Brush);
	
	BackgroundRarity->SetBrushTintColor(FSlateColor(URPGUIStatics::GetColorByRarity(GetWorld(),Definition.RarityTag)));
}

void UItemSlotWidget::EmptySlot()
{
	Super::EmptySlot();
	ItemQuantity = 0;
}

void UItemSlotWidget::OutlineSlot() const
{
	SetIconPadding(false);
	Border->SetColorAndOpacity(FLinearColor(FColor::FromHex("FFFFFFFF")));
}

void UItemSlotWidget::RemoveOutLineSlot()
{
	//Leave
	SetIconPadding(true);
	Border->SetColorAndOpacity(FLinearColor(FColor::FromHex(TEXT("121212FF"))));
}

void UItemSlotWidget::EnableDragOverPreview(EDragOverResult Result)
{
	switch (Result)
	{
	case EDragOverResult::Drop:
		DragOverPreview->SetColorAndOpacity(DragOverPreviewColor);
		break;
	case EDragOverResult::Swap:
		DragOverPreview->SetColorAndOpacity(DragOverPreviewColor);
		break;
	case EDragOverResult::Invalid:
		DragOverPreview->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 0.4f));
		break;
	}
}

void UItemSlotWidget::DisableDragOverPreview()
{
	DragOverPreview->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
}

void UItemSlotWidget::StartSelectedAnimation()
{
	OutlineSlot();
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Blue,FString::Printf(TEXT("Animation start")));
	if (!IsValid(SelectedSlotAnimation)) return;
	PlayAnimation(SelectedSlotAnimation,0.f,0,EUMGSequencePlayMode::Forward);
}

void UItemSlotWidget::StopSelectedAnimation()
{
	RemoveOutLineSlot();
	StopAnimation(SelectedSlotAnimation);
}

void UItemSlotWidget::SetIconPadding(bool reset) const
{
	UOverlaySlot* BorderSlot = Cast<UOverlaySlot>(BackgroundRarity->Slot);
	if (!IsValid(BorderSlot)) return;
    
	if (reset)
	{
		BorderSlot->SetPadding(FMargin(0.f));
		return;
	}
    
	if (CurrentSlotSize == ESlotSizeCategories::UniqueSlot)
	{
		BorderSlot->SetPadding(FMargin(5.f)); // Unique ocupa todo el slot
		return;
	}
    
	if (CurrentSlotSize == ESlotSizeCategories::SuperiorSlotVertical)
	{
		BorderSlot->SetPadding(FMargin(5.f, 5.f, 5.f, 0.f)); // Sin gap abajo
	}
	else // LowerSlot
	{
		BorderSlot->SetPadding(FMargin(5.f, 0.f, 5.f, 5.f)); // Sin gap arriba
	}
}

FReply UItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		OnItemRowClickedDelegate.Execute(CurrentGridIndex);
		//DragAndDrop
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}
	return FReply::Unhandled();
}

void UItemSlotWidget::BuildDragOperation(UDragDropOperation*& OutOperation)
{
	Super::BuildDragOperation(OutOperation);
	
	UItemSlotDragDrogOperation* DragDropOperation = Cast<UItemSlotDragDrogOperation>(
		UWidgetBlueprintLibrary::CreateDragDropOperation(UItemSlotDragDrogOperation::StaticClass()));
	
	UItemDragVisualWidget* DragVisual = CreateWidget<UItemDragVisualWidget>(GetWorld(),IconWidgetClass);
	if (!IsValid(DragVisual)) return;
	
	DragVisual->SetIcon(CurrentItemDefinition.Icon.Get(),CurrentSlotSize);
	
	DragDropOperation->Pivot             = EDragPivot::CenterCenter;
	
	DragDropOperation->ItemDraggedIconWidget = DragVisual;
	DragDropOperation->DefaultDragVisual = DragVisual;
	
	DragDropOperation->Payload           = this;
	DragDropOperation->ItemSlot_Payload  = this;
	
	DragDropOperation->ItemEntry         = &ItemEntry;
	DragDropOperation->SlotSize          = CurrentSlotSize;
	DragDropOperation->ItemDefinition    = CurrentItemDefinition;
	
	OutOperation = DragDropOperation;
	
	OnNewDragOperation.ExecuteIfBound(DragDropOperation);
}

void UItemSlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	//No ha dropeado en ningún lado -> mundo
	if (!IsValid(InOperation)) return;
		
	UItemSlotDragDrogOperation* DragDropOp = Cast<UItemSlotDragDrogOperation>(InOperation);
	if (!IsValid(DragDropOp)) return;
	
	if (IsValid(DragDropOp->LastEnterSlotWidget))
		DragDropOp->LastEnterSlotWidget->DisableDragOverPreview();

	const int32 FailedIndex = IsValid(DragDropOp->LastEnterSlotWidget) 
	   ? DragDropOp->LastEnterSlotWidget->GetGridIndex()
	   : INDEX_NONE;
	
	const FRPGInventoryEntry& Entry = DragDropOp->LastEnterSlotWidget->GetItemEntry();

	OnDragCancelledDelegate.ExecuteIfBound(FailedIndex,DragDropOp->ItemSlot_Payload->GetGridIndex(), DragDropOp->SlotSize);
}

void UItemSlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	
	UItemSlotDragDrogOperation* DragDropOp = Cast<UItemSlotDragDrogOperation>(InOperation);
	if (!IsValid(DragDropOp)) return;

	const bool bIsValid = IsItemValidForThisSlot(DragDropOp->ItemDefinition);
	const bool bFromEquipmentSlot = !IsValid(DragDropOp->ItemSlot_Payload);
	const int32 FromIndex = bFromEquipmentSlot ? INDEX_NONE : DragDropOp->ItemSlot_Payload->GetGridIndex();
	const ESlotSizeCategories DroppedSize = DragDropOp->SlotSize;
	
	OnNewDragOperation.ExecuteIfBound(DragDropOp);
	DragDropOp->LastEnterSlotWidget = this;
	
	if (DroppedSize == ESlotSizeCategories::UniqueSlot)
	{
		if (!bIsValid)
		{
			EnableDragOverPreview(EDragOverResult::Invalid);
			DragDropOp->ItemDraggedIconWidget->EnableDragOverResultIcon(EDragOverResult::Invalid);
			return;	
		}
		
		if (bIsEmpty)
		{
			// Empty target → can drop
			EnableDragOverPreview(EDragOverResult::Drop);
			DragDropOp->ItemDraggedIconWidget->EnableDragOverResultIcon(EDragOverResult::Drop);
		}
		else if (CurrentSlotSize == ESlotSizeCategories::UniqueSlot)
		{
			DragDropOp->ItemDraggedIconWidget->EnableDragOverResultIcon(EDragOverResult::Swap);
			EnableDragOverPreview(EDragOverResult::Swap);
		}
		else
		{
			// Target is Superior or Lower → invalid
			EnableDragOverPreview(EDragOverResult::Invalid);
			DragDropOp->ItemDraggedIconWidget->EnableDragOverResultIcon(EDragOverResult::Invalid);
		}
		return;
	}

	//int32 NormalizedTargetIndex = CurrentGridIndex;

	// Siempre normalizar al Superior, sea cual sea el slot que se entre
	// if (CurrentSlotSize == ESlotSizeCategories::LowerSlotVertical)
	// {
	// 	NormalizedTargetIndex = FMath::Max(0, CurrentGridIndex - MaxColumns);
	// }
	
	const EDragOverResult Result = bIsValid ? EDragOverResult::Drop : EDragOverResult::Invalid;
	OnDragEnteredDelegate.ExecuteIfBound(FromIndex,CurrentGridIndex,Result);
}

void UItemSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	UItemSlotDragDrogOperation* DragDropOp = Cast<UItemSlotDragDrogOperation>(InOperation);
	if (!IsValid(DragDropOp)) return;

	const bool bIsValid = IsItemValidForThisSlot(DragDropOp->ItemDefinition);
	const bool bFromEquipmentSlot = !IsValid(DragDropOp->ItemSlot_Payload);
	const int32 FromIndex = bFromEquipmentSlot ? INDEX_NONE : DragDropOp->ItemSlot_Payload->GetGridIndex();
	const ESlotSizeCategories DroppedSize = DragDropOp->SlotSize;

	DragDropOp->ItemDraggedIconWidget->DisableDragOverResultIcon();
	
	if (DroppedSize == ESlotSizeCategories::UniqueSlot)
	{
		DisableDragOverPreview();
		DragDropOp->ItemDraggedIconWidget->DisableDragOverResultIcon();
		return;
	}

	// int32 NormalizedTargetIndex = CurrentGridIndex;
	// if (CurrentSlotSize == ESlotSizeCategories::LowerSlotVertical)
	// {
	// 	NormalizedTargetIndex = CurrentGridIndex - MaxColumns;
	// }

	const EDragOverResult Result = bIsValid ? EDragOverResult::Drop : EDragOverResult::Invalid;
	OnDragLeavedDelegate.ExecuteIfBound(FromIndex,CurrentGridIndex,Result);
}

bool UItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UItemSlotDragDrogOperation* DragDropOp = Cast<UItemSlotDragDrogOperation>(InOperation);
	if (!IsValid(DragDropOp)) return false;

	const bool bIsValid = IsItemValidForThisSlot(DragDropOp->ItemDefinition);
	const EDragOverResult Result = bIsValid ? EDragOverResult::Drop : EDragOverResult::Invalid;
	const bool bFromEquipmentSlot = !IsValid(DragDropOp->ItemSlot_Payload);
	const int32 FromIndex = bFromEquipmentSlot ? INDEX_NONE : DragDropOp->ItemSlot_Payload->GetGridIndex();
	
	const ESlotSizeCategories DroppedSize = DragDropOp->SlotSize;
	
	if (DroppedSize == ESlotSizeCategories::UniqueSlot)
	{
		if (!bIsValid)
		{
			return false;
		}
		if (bIsEmpty)
		{
			// Move to empty slot
			OnItemDroppedPanelDelegate.ExecuteIfBound(FromIndex, CurrentGridIndex,Result);
			DisableDragOverPreview();
			//DisableDragOverResultIcon();
			DragDropOp->ItemDraggedIconWidget->DisableDragOverResultIcon();
			return true;
		}
		if (CurrentSlotSize == ESlotSizeCategories::UniqueSlot)
		{
			// Swap two unique slots
			OnItemDroppedPanelDelegate.ExecuteIfBound(FromIndex, CurrentGridIndex,Result);
			DisableDragOverPreview();
			//DisableDragOverResultIcon();
			DragDropOp->ItemDraggedIconWidget->DisableDragOverResultIcon();
			return true;
		}
		// Target is Superior or Lower → reject
		return false;
	} 

	if (CurrentSlotSize == ESlotSizeCategories::UniqueSlot && !bIsEmpty)
	{
		// Can't place a 2-slot item on top of a single unique item
		return false;
	}
	if (!OnItemDroppedPanelDelegate.IsBound())
    {
        UE_LOG(LogTemp, Warning, TEXT("Delegate NOT bound"));
    }

	// int32 NormalizedTargetIndex = CurrentGridIndex;
	// if (DroppedSize != ESlotSizeCategories::UniqueSlot && 
	// 	CurrentSlotSize == ESlotSizeCategories::LowerSlotVertical)
	// {
	// 	NormalizedTargetIndex = CurrentGridIndex - MaxColumns;
	// }
	
	OnItemDroppedPanelDelegate.ExecuteIfBound(FromIndex,CurrentGridIndex,Result);
	return true;
}

bool UItemSlotWidget::IsItemValidForThisSlot(const FMasterItemDefinition& Definition) const
{
	if (!AcceptedSubCategoryTag.IsValid()) return true;
	return Definition.SubcategoryTag.MatchesTag(AcceptedSubCategoryTag);
}
