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

	SetIcon();
}

void UItemSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	OnItemRowClickedDelegate.Unbind();
	OnItemDroppedEventDelegate.Unbind();

	Super::NativeDestruct();
}

void UItemSlotWidget::Init(const FRPGInventoryEntry& Entry,TSoftObjectPtr<UTexture2D> Icon)
{
	ItemEntry = Entry;
	SetItemNameText(ItemEntry.ItemName);
	SetQuantityText(ItemEntry.Quantity);
	
	SoftIconTexture = Icon;
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

	OnItemDroppedEventDelegate.ExecuteIfBound(*DragDropOp->ItemEntry);
}

void UItemSlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Drag Enter")));
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UItemSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Emerald,FString::Printf(TEXT("Drag Leave")));
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

