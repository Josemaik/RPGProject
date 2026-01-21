// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemSlotWidget.h"

#include "GameplayTagContainer.h"
#include "AbilitySystem/RPGGameplayTags.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemSlotDroppedDragDrop.h"
#include "UI/Inventory/ItemSlotIcon.h"
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

void UItemSlotWidget::SetIcon()
{
	IconWidgetReference = Cast<UItemSlotIcon>(CreateWidget(this,IconWidgetClass));
	if (IsValid(IconWidgetReference))
	{
		IconWidgetReference->SetIcon(SoftIconTexture);
		IconBox->AddChild(IconWidgetReference);
	}
}

void UItemSlotWidget::SetGridSlot(const int32 Index,UUniformGridSlot* NewGridSlot)
{
	GridSlot = NewGridSlot;
	GridIndex = Index;
}

void UItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIcon();
}

void UItemSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	OnUseButtomClickedDelegate.Unbind();
	OnItemRowClickedDelegate.Unbind();
	OnItemDroppedEventDelegate.Unbind();
	RemoveFromParent();
	
	if (WidgetController)
	{
		WidgetController->InventoryEntryDelegate.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UItemSlotWidget::Init(const FRPGInventoryEntry& Entry,TSoftObjectPtr<UTexture2D> Icon,TObjectPtr<UInventoryWidgetController> InventoryWidgetController)
{
	ItemEntry = Entry;
	SetItemNameText(ItemEntry.ItemName);
	SetQuantityText(ItemEntry.Quantity);
	
	SoftIconTexture = Icon;
	
	WidgetController = InventoryWidgetController;

	if (IsValid(WidgetController))
	{
		WidgetController->InventoryEntryDelegate.AddUObject(
			this,
			&UItemSlotWidget::OnItemUpdated
		);
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
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	UItemSlotDroppedDragDrop* DragDropOperation = Cast<UItemSlotDroppedDragDrop>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemSlotDroppedDragDrop::StaticClass()));
	DragDropOperation->Pivot = EDragPivot::CenterCenter;
	DragDropOperation->DefaultDragVisual = IconWidgetReference;
	DragDropOperation->Payload = this;
	DragDropOperation->ItemEntry = &ItemEntry;

	OutOperation = DragDropOperation;
}

void UItemSlotWidget::OnItemUpdated(const FRPGInventoryEntry& UpdatedEntry)
{
	if (UpdatedEntry.ItemID != ItemEntry.ItemID) return;

	SetQuantityText(UpdatedEntry.Quantity);
}
