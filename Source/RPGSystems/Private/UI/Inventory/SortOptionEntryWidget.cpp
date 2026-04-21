// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/SortOptionEntryWidget.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

void USortOptionEntryWidget::SetOptionText(FText InOptionText)
{
	if (!IsValid(OptionText)) return;
	OptionText->SetText(InOptionText);
}

void USortOptionEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OptionButton->OnClicked.AddDynamic(this,&USortOptionEntryWidget::OnOptionButtonClicked);
}

void USortOptionEntryWidget::OnOptionButtonClicked()
{
	if (!IsValid(CheckBox) || !IsValid(OptionButton)) return;
	
	CheckBox->SetCheckedState(ECheckBoxState::Checked);
	
	OnButtonClicked.ExecuteIfBound();
}
