// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/ItemCategoryButton.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemCategoryButton::Init(FGameplayTag Category, FText CategoryText, UTexture2D* IconTexture)
{
	CategoryTag = Category;
	Icon->SetBrushFromTexture(IconTexture);
	Icon->SetDesiredSizeOverride(FVector2d(32.f,32.f));
}

void UItemCategoryButton::NativeConstruct()
{
	Super::NativeConstruct();

	ActionButton->OnClicked.AddDynamic(this,&UItemCategoryButton::OnActionButtonClicked);
	ActionButton->OnHovered.AddDynamic(this,&UItemCategoryButton::OnActionButtonHovered);
	ActionButton->OnUnhovered.AddDynamic(this,&UItemCategoryButton::OnActionButtonUnhovered);
}

void UItemCategoryButton::OnActionButtonHovered()
{
	Icon->SetBrushTintColor(FSlateColor(HoverColor));
}

void UItemCategoryButton::OnActionButtonUnhovered()
{
	Icon->SetBrushTintColor(FSlateColor(UnhoverColor));
}

void UItemCategoryButton::OnActionButtonClicked()
{
	OnCategorySelected.Broadcast(CategoryTag);
}
