// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/SortOptionEntryWidget.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

void USortOptionEntryWidget::SetOption(EItemSortType SortType)
{
	if (!IsValid(OptionText)) return;
	
	FString SrtTypeEnum = UEnum::GetValueAsString(SortType);
	FString SortTypeName;
	SrtTypeEnum.Split(TEXT("::"), nullptr, &SortTypeName);

	Option = SortType;
	OptionText->SetText(FText::FromString(SortTypeName));
}

void USortOptionEntryWidget::UnCheck() const
{
	if (!IsValid(CheckBox)) return;
	CheckBox->SetCheckedState(ECheckBoxState::Unchecked);
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
	
	OnButtonClicked.ExecuteIfBound(this,Option);
}
