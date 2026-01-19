// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/CategoryButton.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCategoryButton::Init(FGameplayTag Category, FText CategoryText, UTexture2D* IconTexture)
{
	CategoryTag = Category;
	ItemCategoryName->SetText(CategoryText);
	Icon->SetBrushFromTexture(IconTexture);
	Icon->SetDesiredSizeOverride(FVector2d(32.f,32.f));
}

void UCategoryButton::NativeConstruct()
{
	Super::NativeConstruct();

	ActionButton->OnClicked.AddDynamic(this,&UCategoryButton::OnActionButtonClicked);
}

void UCategoryButton::OnActionButtonClicked()
{
	OnCategorySelected.Broadcast(CategoryTag);
}
