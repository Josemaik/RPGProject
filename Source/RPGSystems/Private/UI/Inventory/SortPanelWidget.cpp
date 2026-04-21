// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/SortPanelWidget.h"

#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "UI/Inventory/SortOptionEntryWidget.h"

void USortPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(OptionsEntryVerticalBox)) return;
	
	for (int32 i = 0; i < static_cast<int32>(EItemSortType::MAX); ++i)
	{
		USortOptionEntryWidget* OptionEntryWidget = CreateWidget<USortOptionEntryWidget>(this,SortOptionEntryClass);
		if (!IsValid(OptionEntryWidget)) return;
		
		EItemSortType Option = static_cast<EItemSortType>(i);
		
		FString FullName = UEnum::GetValueAsString(Option);
		FString ShortName;
		FullName.Split(TEXT("::"), nullptr, &ShortName);
		
		OptionEntryWidget->SetOptionText(FText::FromString(ShortName));

		OptionEntryWidget->OnButtonClicked.BindLambda([this,Option]
		{
			OnOptionChanged.ExecuteIfBound(Option);
		});

		OptionsEntryVerticalBox->AddChild(OptionEntryWidget);
	}
	
	CloseButton->OnClicked.AddDynamic(this, &USortPanelWidget::OnCloseButtonClick);
}

void USortPanelWidget::OnCloseButtonClick()
{
	OnCloseButtonClicked.ExecuteIfBound();
}
