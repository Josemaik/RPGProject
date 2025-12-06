// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemRowIcon.h"

#include "Components/Image.h"

void UItemRowIcon::SetIcon(TSoftObjectPtr<UTexture2D>& Texture)
{
	Icon->SetBrushFromSoftTexture(Texture);
}
