// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemSlotWidget.h"

#include "GameplayTagContainer.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemSlotDroppedDragDrop.h"
#include "UI/Inventory/ItemSlotIcon.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Misc/Iteration.h"
#include "UI/WidgetController/InventoryWidgetController.h"


void UItemSlotWidget::SetItemNameText(FText Text)
{
	//ItemName->SetText(Text);
}

void UItemSlotWidget::SetQuantityText(int32 Quantity)
{
	if (!IsValid(this)) return;
	
	if (!ItemEntry.ItemTag.IsValid() || ItemEntry.ItemTag.MatchesTag(RPGGameplayTags::InventoryItems::EquipmentTag)) return;

	if (!ItemQuantity) return;
	
	const FText FormatText = FText::FromString(FString("x") + FString::FromInt(Quantity));
	ItemQuantity->SetText(FormatText);
}

void UItemSlotWidget::SetIcon(const FSlateBrush& Brush)
{
	IconWidgetReference = Cast<UItemSlotIcon>(CreateWidget(this,IconWidgetClass));
	if (IsValid(IconWidgetReference))
	{
		IconWidgetReference->SetIcon(SoftIconTexture);
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
	OnItemDroppedEventDelegate.Unbind();

	Super::NativeDestruct();
}

void UItemSlotWidget::Init(const FRPGInventoryEntry& Entry,const TSoftObjectPtr<UTexture2D>& Icon,const FSlateBrush& Brush,ESlotSizeCategories SlotSize)
{
	ItemEntry = Entry;
	SetItemNameText(ItemEntry.ItemName);
	SetQuantityText(ItemEntry.Quantity);

	SoftIconTexture = Icon;
	SetIcon(Brush);
	
	CurrentIconBrush = Brush;
	
	bIsEmpty = false;
	CurrentSlotSize = SlotSize;

	SetIconPadding();
}

void UItemSlotWidget::EmptySlot()
{
	ItemEntry = FRPGInventoryEntry();
	ItemQuantity = 0;
	IconBox->SetColorAndOpacity(FLinearColor::Black);
	IconBox->SetBrushFromTexture(nullptr);
	CurrentIconBrush = FSlateBrush();
	bIsEmpty = true;
	
	if (UOverlaySlot* IconBoxSlot = Cast<UOverlaySlot>(IconBox->Slot))
	{
		IconBoxSlot->SetPadding(FMargin(5.f, 5.f, 5.f, 5.f));
	}
}

void UItemSlotWidget::OutlineSlot(ESlotSizeCategories SlotSize)
{
	CurrentSlotSize = SlotSize;
	SetIconPadding();
	Border->SetColorAndOpacity(FColor::Orange);
}

void UItemSlotWidget::SetIconPadding() const
{
	if (!IsValid(IconBox) || CurrentSlotSize == ESlotSizeCategories::UniqueSlot) return;
	UOverlaySlot* IconBoxSlot = Cast<UOverlaySlot>(IconBox->Slot);
	if (!IsValid(Slot)) return;
	
	if (CurrentSlotSize == ESlotSizeCategories::SuperiorSlotVertical)
	{
		IconBoxSlot->SetPadding(FMargin(5.f, 5.f, 5.f, 0));
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
	DragDropOperation->IconTexture = SoftIconTexture.Get();
	
	OutOperation = DragDropOperation;
}

void UItemSlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Drag Cancelled")));
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	//No ha dropeado en ningún lado -> mundo
	if (!IsValid(InOperation)) return;
		
	UItemSlotDroppedDragDrop* DragDropOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragDropOp)) return;
	UItemSlotWidget* DraggedSlot = DragDropOp->ItemSlot_Payload;
	if (!IsValid(DraggedSlot)) return;
	
	OnItemDroppedEventDelegate.ExecuteIfBound(DraggedSlot->ItemEntry);
}

void UItemSlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Drag Enter")));
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	
	UItemSlotDroppedDragDrop* DragDropOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragDropOp)) return;
	
	UItemSlotWidget* DroppedItem = DragDropOp->ItemSlot_Payload;
	if (!IsValid(DroppedItem)) return;

	if (DroppedItem == this) return; //Ignorar a mi mismo
	
	OutlineSlot(DroppedItem->GetCurrentSlotSize());
	
	OnDragEnteredDelegate.ExecuteIfBound(CurrentGridIndex);
}

void UItemSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Drag Leave")));
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	if (InOperation->Payload == this) return; //Ignorar a mi mismo
	
	if (UOverlaySlot* IconBoxSlot = Cast<UOverlaySlot>(IconBox->Slot))
	{
		IconBoxSlot->SetPadding(FMargin(5.f, 5.f, 5.f, 5.f));
	}
	Border->SetColorAndOpacity(FColor::Black);
}

bool UItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Dro Item Slot")));
	UItemSlotDroppedDragDrop* DragDropOp = Cast<UItemSlotDroppedDragDrop>(InOperation);
	if (!IsValid(DragDropOp)) return false;

	UItemSlotWidget* DroppedItem = DragDropOp->ItemSlot_Payload;
	if (!IsValid(DroppedItem)) return false;
	
	//Add to the new slot
	//Clear the last slot
	// if (!IsValid(DragDropOp->ItemSlot_Payload)) return false;
	// DragDropOp->ItemSlot_Payload->EmptySlot();

	if (DroppedItem->GetCurrentSlotSize() != ESlotSizeCategories::UniqueSlot)
	{
		OnItemDroppedPanelDelegate.ExecuteIfBound(DroppedItem,this);
	}
	
	if (!IsValid(Border)) return false;
	Border->SetColorAndOpacity(FColor::Black); 
	
	return true;
}