// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemSlotIcon.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "UI/Inventory/SlotSizeCategories.h"

void UItemSlotIcon::SetIcon(TObjectPtr<UTexture2D> IconTexture, ESlotSizeCategories Size)
{
	FVector2f IconSize = (Size == LowerSlotVertical || Size == SuperiorSlotVertical) ?
		IconSizeTwoSlots : IconSizeUniqueSlot;
	
	SizeBox->SetWidthOverride(IconSize.X);
	SizeBox->SetHeightOverride(IconSize.Y);
	Icon->SetBrushFromTexture(IconTexture);
}
