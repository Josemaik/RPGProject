// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/CategoryButton.h"

#include "Components/Button.h"

void UCategoryButton::NativeConstruct()
{
	Super::NativeConstruct();

	ActionButtom->OnClicked.AddDynamic(this,&UCategoryButton::OnActionButtonClicked);
}

void UCategoryButton::OnActionButtonClicked()
{
	OnCategorySelected.Broadcast(CategoryTag);
}
