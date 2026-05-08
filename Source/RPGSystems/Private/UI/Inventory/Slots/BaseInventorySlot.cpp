// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/Slots/BaseInventorySlot.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Libraries/RPGUIStatics.h"
#include "UI/Inventory/Slots/SlotSizeCategories.h"
#include "UI/Inventory/ItemDragVisualWidget.h"

void UBaseInventorySlot::NativeConstruct()
{
    Super::NativeConstruct();
}

void UBaseInventorySlot::CancelLeaveTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(ItemTooltipLeaveTimerHandle);
}

void UBaseInventorySlot::SetIcon(const FSlateBrush& Brush)
{
    // IconWidgetReference = Cast<UItemDragVisualWidget>(CreateWidget(this, IconWidgetClass));
    // if (IsValid(IconWidgetReference))
    // {
    //     IconWidgetReference->SetIcon(CurrentItemDefinition.Icon.Get(), CurrentSlotSize);
    // }
    if (!IsValid(IconBox)) return;
    IconBox->SetBrush(Brush);
    IconBox->SetColorAndOpacity(FLinearColor::White);
}

void UBaseInventorySlot::EquipVisual(UTexture2D* Texture, FGameplayTag RarityTag)
{
    if (!IsValid(IconBox)) return;
    IconBox->SetBrushFromTexture(Texture);
    IconBox->SetColorAndOpacity(FLinearColor::White);
    if (IsValid(BackgroundRarity))
    {
        BackgroundRarity->SetOpacity(1.f);
        BackgroundRarity->SetBrushTintColor(URPGUIStatics::GetColorByRarity(GetWorld(), RarityTag));
    }
    bIsEmpty = false;
}

void UBaseInventorySlot::EmptySlot()
{
    ItemEntry = FRPGInventoryEntry();
    if (IsValid(IconBox))
    {
        IconBox->SetColorAndOpacity(FLinearColor(FColor::FromHex(TEXT("121212FF"))));
        IconBox->SetBrushFromTexture(nullptr);
    }
    CurrentIconBrush = FSlateBrush();
    bIsEmpty = true;
    CurrentSlotSize = ESlotSizeCategories::UniqueSlot;
}

void UBaseInventorySlot::StartSelectedAnimation()
{
    if (!IsValid(SelectedSlotAnimation)) return;
    PlayAnimation(SelectedSlotAnimation, 0.f, 0, EUMGSequencePlayMode::Forward);
}

void UBaseInventorySlot::StopSelectedAnimation()
{
    StopAnimation(SelectedSlotAnimation);
}

// Tooltip

void UBaseInventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    GetWorld()->GetTimerManager().ClearTimer(ItemTooltipLeaveTimerHandle);

    if (IsValid(LinkedSlot))
        LinkedSlot->CancelLeaveTimer();

    const float Delay = bIsEmpty ? 0.f : TooltipDelay;
    GetWorld()->GetTimerManager().SetTimer(ItemTooltipTimerHandle, [this]
    {
        OnSlotMouseEnteredDelegate.ExecuteIfBound(this);
    }, Delay, false);
}

void UBaseInventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    GetWorld()->GetTimerManager().ClearTimer(ItemTooltipTimerHandle);

    GetWorld()->GetTimerManager().SetTimer(ItemTooltipLeaveTimerHandle, [this]
    {
        OnSlotMouseLeavedDelegate.ExecuteIfBound(this);
    }, 0.05f, false);
}

// Drag

FReply UBaseInventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        GetWorld()->GetTimerManager().ClearTimer(ItemTooltipTimerHandle);
        return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }
    return FReply::Unhandled();
}

void UBaseInventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                               UDragDropOperation*& OutOperation)
{
    if (bIsEmpty) return;
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
    BuildDragOperation(OutOperation);
}