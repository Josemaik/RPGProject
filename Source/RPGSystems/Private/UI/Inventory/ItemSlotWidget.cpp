// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemSlotWidget.h"

#include "GameplayTagContainer.h"
#include "AbilitySystem/NativeTags/RPGInventoryTags.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemSlotDroppedDragDrop.h"
#include "UI/Inventory/ItemSlotIcon.h"
#include "Components/OverlaySlot.h"
#include "Libraries/RPGUIStatics.h"


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
		IconWidgetReference->SetIcon(CurrentItemDefinition.Icon.Get(),CurrentSlotSize);
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

void UItemSlotWidget::Init(const FRPGInventoryEntry& Entry,const FMasterItemDefinition& Definition,FSlateBrush Brush, ESlotSizeCategories SlotSize)
{
	ItemEntry = Entry;
	CurrentItemDefinition = Definition;
	SetQuantityText(ItemEntry.Quantity);
	
	bIsEmpty = false;
	CurrentSlotSize = SlotSize;

	CurrentIconBrush = Brush;
	//SoftIconTexture = Definition.Icon;
	SetIcon(Brush);

	//CurrentRarity = Definition.Rarity;
	BackgroundRarity->SetBrushTintColor(FSlateColor(URPGUIStatics::GetColorForRarity(GetWorld(),Definition.Rarity)));
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

void UItemSlotWidget::EnableDragOverResultIcon(EDragOverResult Result,ESlotSizeCategories DraggedSize)
{
	const bool bIsLastColumn = (CurrentGridIndex % MaxColumns) == (MaxColumns - 1);
	float TranslationX = bIsLastColumn ? -76.f : 31.f;
	float TranslationY = DraggedSize == UniqueSlot ? -25.f : 35.f;
	FVector2D Translation = FVector2D(TranslationX, TranslationY);
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
	DragOverResultIcon->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemSlotWidget::StartSelectedAnimation()
{
	OutlineSlot();
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
		//GetWorld()->GetTimerManager().ClearTimer(ItemTooltipTimerHandle);
		OnItemRowClickedDelegate.Execute(CurrentGridIndex);
		//DragAndDrop
		//return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}
	return FReply::Unhandled();
}

void UItemSlotWidget::BuildDragOperation(UDragDropOperation*& OutOperation)
{
	Super::BuildDragOperation(OutOperation);
	
	UItemSlotDroppedDragDrop* DragDropOperation = Cast<UItemSlotDroppedDragDrop>(
		UWidgetBlueprintLibrary::CreateDragDropOperation(UItemSlotDroppedDragDrop::StaticClass()));
	DragDropOperation->Pivot             = EDragPivot::CenterCenter;
	DragDropOperation->DefaultDragVisual = IconWidgetReference;
	DragDropOperation->Payload           = this;
	DragDropOperation->ItemSlot_Payload  = this;
	DragDropOperation->ItemEntry         = &ItemEntry;
	DragDropOperation->SlotSize          = CurrentSlotSize;
	//DragDropOperation->IconTexture       = SoftIconTexture.Get();
	//DragDropOperation->Rarity            = CurrentRarity;
	DragDropOperation->ItemDefinition    = CurrentItemDefinition;
	OutOperation = DragDropOperation;
}

// void UItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
//                                            UDragDropOperation*& OutOperation)
// {
// 	if (bIsEmpty) return;
// 	
// 	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
// 	
// 	BuildDragOperation(OutOperation);
// }

void UItemSlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
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
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,FString::Printf(TEXT("Entra Drag")));
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	
	UItemSlotDroppedDragDrop* DragDropOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragDropOp)) return;

	const bool bFromEquipmentSlot = !IsValid(DragDropOp->ItemSlot_Payload);
	const int32 FromIndex = bFromEquipmentSlot ? INDEX_NONE : DragDropOp->ItemSlot_Payload->GetGridIndex();
	const ESlotSizeCategories DroppedSize = DragDropOp->SlotSize;

	if (bFromEquipmentSlot)
	{
		OnEquipmentEnteredDelegate.ExecuteIfBound(DragDropOp);
	}
	
	// UItemSlotWidget* DroppedItem = DragDropOp->ItemSlot_Payload;
	// if (!IsValid(DroppedItem)) return;

	DragDropOp->LastEnterSlotWidget = this;
	
	if (DroppedSize == ESlotSizeCategories::UniqueSlot)
	{
		if (bIsEmpty)
		{
			// Empty target → can drop
			EnableDragOverPreview(EDragOverResult::Drop);
			EnableDragOverResultIcon(EDragOverResult::Drop,DragDropOp->SlotSize);
		}
		else if (CurrentSlotSize == ESlotSizeCategories::UniqueSlot)
		{
			EnableDragOverResultIcon(EDragOverResult::Swap,DragDropOp->SlotSize);
			EnableDragOverPreview(EDragOverResult::Swap);
		}
		else
		{
			// Target is Superior or Lower → invalid
			EnableDragOverPreview(EDragOverResult::Invalid);
			EnableDragOverResultIcon(EDragOverResult::Invalid,DragDropOp->SlotSize);
		}
		return;
	}
	
	OnDragEnteredDelegate.ExecuteIfBound(FromIndex,CurrentGridIndex);
}

void UItemSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	UItemSlotDroppedDragDrop* DragDropOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragDropOp)) return;

	const bool bFromEquipmentSlot = !IsValid(DragDropOp->ItemSlot_Payload);
	const int32 FromIndex = bFromEquipmentSlot ? INDEX_NONE : DragDropOp->ItemSlot_Payload->GetGridIndex();
	const ESlotSizeCategories DroppedSize = DragDropOp->SlotSize;
	// UItemSlotWidget* DroppedItem = DragDropOp->ItemSlot_Payload;
	// if (!IsValid(DroppedItem)) return;
	
	if (DroppedSize == ESlotSizeCategories::UniqueSlot)
	{
		// Only this slot was highlighted
		DisableDragOverPreview();
		DisableDragOverResultIcon();
		return;
	}
	
	OnDragLeavedDelegate.ExecuteIfBound(FromIndex,CurrentGridIndex);
}

bool UItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UItemSlotDroppedDragDrop* DragDropOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragDropOp)) return false;
	
	// UItemSlotWidget* DroppedItem = DragDropOp->ItemSlot_Payload;
	// if (!IsValid(DroppedItem)) return false;
	const bool bFromEquipmentSlot = !IsValid(DragDropOp->ItemSlot_Payload);
	const int32 FromIndex = bFromEquipmentSlot ? INDEX_NONE : DragDropOp->ItemSlot_Payload->GetGridIndex();
	
	const ESlotSizeCategories DroppedSize = DragDropOp->SlotSize;
	
	if (DroppedSize == ESlotSizeCategories::UniqueSlot)
	{
		if (bIsEmpty)
		{
			// Move to empty slot
			OnItemDroppedPanelDelegate.ExecuteIfBound(FromIndex, CurrentGridIndex);
			DisableDragOverPreview();
			DisableDragOverResultIcon();
			return true;
		}
		if (CurrentSlotSize == ESlotSizeCategories::UniqueSlot)
		{
			// Swap two unique slots
			OnItemDroppedPanelDelegate.ExecuteIfBound(FromIndex, CurrentGridIndex);
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
	
	OnItemDroppedPanelDelegate.ExecuteIfBound(FromIndex,CurrentGridIndex);
	return true;
}

// void UItemSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
// {
// 	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
//
// 	// Cancelar mi propio leave pendiente
// 	GetWorld()->GetTimerManager().ClearTimer(ItemTooltipLeaveTimerHandle);
//
// 	// Cancelar el leave del compañero si existe
// 	if (IsValid(LinkedSlot))
// 	{
// 		LinkedSlot->CancelLeaveTimer();
// 	}
//
// 	float InRate = bIsEmpty ? 0.f : 0.5f;
// 	GetWorld()->GetTimerManager().SetTimer(ItemTooltipTimerHandle, [this]
// 	{
// 		OnItemSlotMouseEnteredDelegate.ExecuteIfBound(this);
// 	}, InRate, false);
// }
//
// void UItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
// {
// 	Super::NativeOnMouseLeave(InMouseEvent);
//
// 	GetWorld()->GetTimerManager().ClearTimer(ItemTooltipTimerHandle);
//
// 	GetWorld()->GetTimerManager().SetTimer(ItemTooltipLeaveTimerHandle, [this]
// 	{
// 		OnItemSlotMouseLeavedDelegate.ExecuteIfBound(this);
// 	}, 0.05f, false);
// }
//
// void UItemSlotWidget::CancelLeaveTimer()
// {
// 	GetWorld()->GetTimerManager().ClearTimer(ItemTooltipLeaveTimerHandle);
// }
