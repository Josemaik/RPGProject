// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/SortPanelWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "UI/Inventory/SortOptionEntryWidget.h"

void USortPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(OptionsEntryVerticalBox)) return;
	
	for (EItemSortType Option : TEnumRange<EItemSortType>())
	{
		USortOptionEntryWidget* OptionEntryWidget = CreateWidget<USortOptionEntryWidget>(this,SortOptionEntryClass);
		if (!IsValid(OptionEntryWidget)) return;
		
		OptionEntryWidget->SetOption(Option);

		OptionEntryWidget->OnButtonClicked.BindUObject(this, &USortPanelWidget::OnPressedOptionEntry);

		OptionsEntryVerticalBox->AddChild(OptionEntryWidget);
	}
	
	CloseButton->OnClicked.AddDynamic(this, &USortPanelWidget::OnCloseButtonClick);
}

void USortPanelWidget::OnCloseButtonClick()
{
	OnCloseButtonClicked.ExecuteIfBound();
}

void USortPanelWidget::OnPressedOptionEntry(USortOptionEntryWidget* OptionEntry, EItemSortType Option)
{
	if (IsValid(LastPressedOptionEntryWidget))
	{
		LastPressedOptionEntryWidget->UnCheck();
	}
			
	LastPressedOptionEntryWidget = OptionEntry;
			
	OnOptionChanged.ExecuteIfBound(Option);
}
