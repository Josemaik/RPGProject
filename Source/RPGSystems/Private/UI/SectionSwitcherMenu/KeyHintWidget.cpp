// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SectionSwitcherMenu/KeyHintWidget.h"

#include "Components/TextBlock.h"

void UKeyHintWidget::SetKeyHint(FText InKey, FText InAction) const
{
	if (!IsValid(KeyHintTextBlock) || !IsValid(ActionTextBlock)) return;
	
	KeyHintTextBlock->SetText(InKey);
	ActionTextBlock->SetText(InAction);
}
