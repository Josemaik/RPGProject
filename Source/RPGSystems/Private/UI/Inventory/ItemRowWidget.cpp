// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemRowWidget.h"

#include "GameDelegates.h"
#include "GameplayTagContainer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/Inventory/ItemRowDroppedDragDrop.h"
#include "UI/Inventory/ItemRowIcon.h"

void UItemRowWidget::SetActionText(const FGameplayTag& GameplayTag)
{
	if (GameplayTag.MatchesTag(GameplayTagConsumable))
	{
		ActionText->SetText(FText::FromString("Use"));
		return;
	}
	
	if (GameplayTag.MatchesTag(GameplayTagEquipment))
	{
		ActionText->SetText(FText::FromString("Equip"));
		return;
	}
	
	ActionText->SetText(FText::FromString("Not a equipment"));
	ActionButtom->SetIsEnabled(false); //Disable buttom
}

void UItemRowWidget::SetItemNameText(FText Text)
{
	ItemName->SetText(Text);
}

void UItemRowWidget::SetQuantityText(int32 Quantity)
{
	const FText FormatText = FText::FromString(FString("x") + FString::FromInt(Quantity));
	ItemQuantity->SetText(FormatText);
}

void UItemRowWidget::SetIcon()
{
	IconWidgetReference = Cast<UItemRowIcon>(CreateWidget(this,IconWidgetClass));
	if (IsValid(IconWidgetReference))
	{
		IconWidgetReference->SetIcon(SoftIconTexture);
		IconBox->AddChild(IconWidgetReference);
	}
}

void UItemRowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ActionButtom->OnClicked.AddDynamic(this,&UItemRowWidget::OnClickedUseButtom);

	SetIcon();
}

void UItemRowWidget::SetInventoryEntry(const FRPGInventoryEntry& Entry,TSoftObjectPtr<UTexture2D> Icon)
{
	ItemEntry = Entry;
	SoftIconTexture = Icon;
}

FReply UItemRowWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		OnItemRowClickedDelegate.ExecuteIfBound(ItemEntry);
		//DragAndDrop
		FEventReply ReplyResult = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent,this, EKeys::LeftMouseButton);
		return ReplyResult.NativeReply;
	}
	return FReply::Unhandled();
}

void UItemRowWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	UItemRowDroppedDragDrop* DragDropOperation = Cast<UItemRowDroppedDragDrop>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemRowDroppedDragDrop::StaticClass()));
	DragDropOperation->Pivot = EDragPivot::CenterCenter;
	DragDropOperation->DefaultDragVisual = IconWidgetReference;
	DragDropOperation->Payload = this;

	OutOperation = DragDropOperation;
}

void UItemRowWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	UItemRowWidget* IntemRow =  Cast<UItemRowWidget>(InOperation->Payload);
	if (IsValid(IntemRow))
	{
		OnItemDroppedEventDelegate.ExecuteIfBound(IntemRow->ItemEntry);
	}
}

void UItemRowWidget::OnClickedUseButtom()
{
	OnUseButtomClickedDelegate.ExecuteIfBound(ItemEntry);
}
