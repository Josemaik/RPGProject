// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemSlotWidget.h"

#include "GameplayTagContainer.h"
#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemSlotDroppedDragDrop.h"
#include "UI/Inventory/ItemSlotIcon.h"
#include "Components/OverlaySlot.h"


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
	IconWidgetReference = Cast<UItemSlotIcon>(CreateWidget(this,IconWidgetClass));
	if (IsValid(IconWidgetReference))
	{
		IconWidgetReference->SetIcon(SoftIconTexture.Get(),CurrentSlotSize);
	}
	
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

void UItemSlotWidget::Init(const FRPGInventoryEntry& Entry,const TSoftObjectPtr<UTexture2D>& Icon,const FSlateBrush& Brush,ESlotSizeCategories SlotSize)
{
	ItemEntry = Entry;
	SetQuantityText(ItemEntry.Quantity);
	
	bIsEmpty = false;
	CurrentSlotSize = SlotSize;

	CurrentIconBrush = Brush;
	SoftIconTexture = Icon;
	SetIcon(Brush);
	SetIconPadding(false);
}

void UItemSlotWidget::EmptySlot()
{
	ItemEntry = FRPGInventoryEntry();
	ItemQuantity = 0;
	IconBox->SetColorAndOpacity(FLinearColor(FColor::FromHex(TEXT("121212FF"))));
	IconBox->SetBrushFromTexture(nullptr);
	CurrentIconBrush = FSlateBrush();
	bIsEmpty = true;
	CurrentSlotSize = ESlotSizeCategories::UniqueSlot;
 
	if (UOverlaySlot* IconBoxSlot = Cast<UOverlaySlot>(IconBox->Slot))
	{
		IconBoxSlot->SetPadding(FMargin(5.f, 5.f, 5.f, 5.f));
	}
}

void UItemSlotWidget::OutlineSlot(ESlotSizeCategories SlotSize)
{
	CurrentSlotSize = SlotSize;
	SetIconPadding(false);
	Border->SetColorAndOpacity(FColor::Orange);
}

void UItemSlotWidget::RemoveOutLineSlot(bool OnDrop)
{
	if (OnDrop) //Drop
	{
		Border->SetColorAndOpacity(FLinearColor(FColor::FromHex(TEXT("121212FF"))));
		return;
	}

	//Leave
	CurrentSlotSize = ESlotSizeCategories::UniqueSlot;
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

void UItemSlotWidget::EnableDragOverResultIcon(EDragOverResult Result,ESlotSizeCategories DraggedSize)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("EnableDragOverResultIcon")));
	FVector2D Translation = DraggedSize == UniqueSlot ? FVector2D(31.0, -25.0) : FVector2D(31.0,35.0);
	DragOverResultIcon->SetRenderTranslation(Translation);
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

void UItemSlotWidget::DisableDragOverResultIcon()
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("DisableDragOverResultIcon")));
	DragOverResultIcon->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemSlotWidget::SetIconPadding(bool reset) const
{
	if (!IsValid(IconBox)) return;
	
	UOverlaySlot* IconBoxSlot = Cast<UOverlaySlot>(IconBox->Slot);
	if (!IsValid(IconBoxSlot)) return;

	if (reset)
	{
		IconBoxSlot->SetPadding(FMargin(5.f, 5.f, 5.f, 5.f));
		return;
	}
	
	if (CurrentSlotSize == ESlotSizeCategories::UniqueSlot) return;
	
	if (CurrentSlotSize == ESlotSizeCategories::SuperiorSlotVertical)
	{
		IconBoxSlot->SetPadding(FMargin(5.f, 5.f, 5.f, 0.f));
	}
	else
	{
		IconBoxSlot->SetPadding(FMargin(5.f, 0.f, 5.f, 5.f));
	}
}

FReply UItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		//OnItemRowClickedDelegate.ExecuteIfBound();
		//DragAndDrop
		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return FReply::Unhandled();
}

void UItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	if (bIsEmpty) return;
	
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	UItemSlotDroppedDragDrop* DragDropOperation = Cast<UItemSlotDroppedDragDrop>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemSlotDroppedDragDrop::StaticClass()));
	DragDropOperation->Pivot = EDragPivot::CenterCenter;
	DragDropOperation->DefaultDragVisual = IconWidgetReference;
	DragDropOperation->Payload = this;
	DragDropOperation->ItemSlot_Payload = this;
	DragDropOperation->ItemEntry = &ItemEntry;
	DragDropOperation->SlotSize = CurrentSlotSize;
	DragDropOperation->IconTexture = SoftIconTexture.Get();
	
	OutOperation = DragDropOperation;

	//Change colour to grey or something like that
}

void UItemSlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Drag Cancelled")));
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	//No ha dropeado en ningún lado -> mundo
	if (!IsValid(InOperation)) return;
		
	UItemSlotDroppedDragDrop* DragDropOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragDropOp)) return;
	
	UItemSlotWidget* DroppedItem = DragDropOp->ItemSlot_Payload;
	if (!IsValid(DroppedItem)) return;
	
	if (!IsValid(DragDropOp->LastEnterSlotWidget)) return;
	if (DroppedItem->GetCurrentSlotSize() == UniqueSlot)
	{
		DragDropOp->LastEnterSlotWidget->DisableDragOverPreview();
		return;
	}

	OnDragCancelledDelegate.ExecuteIfBound(DragDropOp->LastEnterSlotWidget->GetGridIndex());
}

void UItemSlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Drag Enter")));
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	
	UItemSlotDroppedDragDrop* DragDropOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragDropOp)) return;
	
	UItemSlotWidget* DroppedItem = DragDropOp->ItemSlot_Payload;
	if (!IsValid(DroppedItem)) return;

	const ESlotSizeCategories DroppedSize = DragDropOp->SlotSize;

	DragDropOp->LastEnterSlotWidget = this;
	
	if (DroppedSize == ESlotSizeCategories::UniqueSlot)
	{
		if (bIsEmpty)
		{
			// Empty target → can drop
			EnableDragOverPreview(EDragOverResult::Drop);
			EnableDragOverResultIcon(EDragOverResult::Drop,DroppedItem->GetCurrentSlotSize());
		}
		else if (CurrentSlotSize == ESlotSizeCategories::UniqueSlot)
		{
			// Unique → Unique: swap
			if (CurrentGridIndex == DroppedItem->GetGridIndex())
			{
				EnableDragOverResultIcon(EDragOverResult::Drop,DroppedItem->GetCurrentSlotSize());
			}
			else
			{
				EnableDragOverResultIcon(EDragOverResult::Swap,DroppedItem->GetCurrentSlotSize());
			}
			EnableDragOverPreview(EDragOverResult::Drop);
		}
		else
		{
			// Target is Superior or Lower → invalid
			EnableDragOverPreview(EDragOverResult::Invalid);
			EnableDragOverResultIcon(EDragOverResult::Invalid,DroppedItem->GetCurrentSlotSize());
		}
		return;
	}
	
	OnDragEnteredDelegate.ExecuteIfBound(DroppedItem->GetGridIndex(),CurrentGridIndex);
}

void UItemSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Drag Leave")));
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	UItemSlotDroppedDragDrop* DragDropOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragDropOp)) return;
	
	UItemSlotWidget* DroppedItem = DragDropOp->ItemSlot_Payload;
	if (!IsValid(DroppedItem)) return;

	const ESlotSizeCategories DroppedSize = DragDropOp->SlotSize;
	
	if (DroppedSize == ESlotSizeCategories::UniqueSlot)
	{
		// Only this slot was highlighted
		DisableDragOverPreview();
		DisableDragOverResultIcon();
		return;
	}
	
	OnDragLeavedDelegate.ExecuteIfBound(DroppedItem->GetGridIndex(),CurrentGridIndex);
}

bool UItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Dro Item Slot")));
	UItemSlotDroppedDragDrop* DragDropOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragDropOp)) return false;

	UItemSlotWidget* DroppedItem = DragDropOp->ItemSlot_Payload;
	if (!IsValid(DroppedItem)) return false;
	
	const ESlotSizeCategories DroppedSize = DragDropOp->SlotSize;
	
	if (DroppedSize == ESlotSizeCategories::UniqueSlot)
	{
		if (bIsEmpty)
		{
			// Move to empty slot
			OnItemDroppedPanelDelegate.ExecuteIfBound(DroppedItem->GetGridIndex(), CurrentGridIndex);
			DisableDragOverPreview();
			DisableDragOverResultIcon();
			return true;
		}
		if (CurrentSlotSize == ESlotSizeCategories::UniqueSlot)
		{
			// Swap two unique slots
			OnItemDroppedPanelDelegate.ExecuteIfBound(DroppedItem->GetGridIndex(), CurrentGridIndex);
			DisableDragOverPreview();
			DisableDragOverResultIcon();
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

	OnItemDroppedPanelDelegate.ExecuteIfBound(DroppedItem->GetGridIndex(),CurrentGridIndex);
	return true;
}