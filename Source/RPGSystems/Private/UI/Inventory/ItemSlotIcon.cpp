// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemSlotIcon.h"

#include "Components/Image.h"

void UItemSlotIcon::SetIcon(TSoftObjectPtr<UTexture2D>& Texture)
{
	Icon->SetBrushFromSoftTexture(Texture);
}
