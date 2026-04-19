// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SectionSwitcherMenu/InputContextWidget.h"

#include "Components/HorizontalBox.h"
#include "Input/URPGUIInputContextData.h"
#include "UI/Inventory/KeyHintWidget.h"

void UInputContextWidget::SetKeyHints(EUISections Section) const
{
	FSectionKeyHints AuxSectionKeyHint = FSectionKeyHints();
	
	for (const FSectionKeyHints& SectionKeyHint :  InputContext->SectionKeyHints)
	{
		if (SectionKeyHint.Section == Section)
		{
			AuxSectionKeyHint = SectionKeyHint;
			break;
		}
	}
	
	HorizontalBox->ClearChildren();

	for (const auto& [Key, Action]: AuxSectionKeyHint.KeyActions)
	{
		UUserWidget* Widget = CreateWidget<UKeyHintWidget>(GetWorld(), KeyHintClass);
		if (!IsValid(Widget)) return;
		
		UKeyHintWidget* KeyHint = Cast<UKeyHintWidget>(Widget);
		if (!IsValid(KeyHint)) return;
		
		KeyHint->SetKeyHint(FText::FromString(Key), FText::FromString(Action));

		if (!IsValid(HorizontalBox)) return;
		
		HorizontalBox->AddChild(KeyHint);
	}
}
