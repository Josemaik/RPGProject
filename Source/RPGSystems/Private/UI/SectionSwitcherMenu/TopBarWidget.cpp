// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SectionSwitcherMenu/TopBarWidget.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "UI/WidgetController/InventoryWidgetController.h"

void UTopBarWidget::InitCarousel(int32 NumSections)
{
	if (!IsValid(HorizontalBoxCarousel)) return;
	
	for (int32 i = 0; i < NumSections; i++)
	{
		USizeBox* SizeBox = NewObject<USizeBox>(this);
		SizeBox->SetWidthOverride(8.f);
		SizeBox->SetHeightOverride(8.f);

		UImage* DotImage = NewObject<UImage>(this);
		SizeBox->AddChild(DotImage);
		
		UHorizontalBoxSlot* HorizontalBoxSlot = HorizontalBoxCarousel->AddChildToHorizontalBox(SizeBox);
		if (!IsValid(HorizontalBoxSlot)) return;
		
		HorizontalBoxSlot->SetPadding(FMargin(4.f, 0.f));
		HorizontalBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));

		//FSlateBrush Brush;
		DotImage->SetBrushFromTexture(CircleTexture,true);
		DotImage->SetColorAndOpacity(InactiveColor);
		
		CarouselImages.Add(DotImage);
	}

	SetActiveSection(0);
}

void UTopBarWidget::UpdateExperience(int Level,float CurrentExperience,float RequiredExperience) const
{
	LevelText->SetText(FText::AsNumber(Level));
	CurrentLevelExperienceText->SetText(FText::AsNumber(CurrentExperience));
	CurrentLevelMaxExperienceText->SetText(FText::AsNumber(RequiredExperience));

	ExperienceBar->SetPercent(CurrentExperience / RequiredExperience);
}

void UTopBarWidget::SetActiveSection(int32 Index)
{
	if (!CarouselImages.IsValidIndex(Index)) return;

	CarouselImages[CurrentIndex]->SetColorAndOpacity(InactiveColor);
	CarouselImages[Index]->SetColorAndOpacity(ActiveColor);
	
	CurrentIndex = Index;
}

void UTopBarWidget::SetSectionData(FString PreviousSection, FString CurrentSection, FString NextSection) const
{
	PreviousSectionText->SetText(FText::FromString(PreviousSection));
	CurrentSectionText->SetText(FText::FromString(CurrentSection));
	NextSectionText->SetText(FText::FromString(NextSection));
}

void UTopBarWidget::SetInventoryWidgetController(UInventoryWidgetController* WidgetController)
{
	if (!IsValid(WidgetController)) return;
	InventoryWidgetControllerRef = WidgetController;

	if (!IsValid(InventoryWidgetControllerRef)) return;
	InventoryWidgetControllerRef->OnInventoryWeightChanged.AddUObject(this,&UTopBarWidget::HandleInventoryWeightChanged);

	MaxInventoryWeightText->SetText(FText::FromString(FString::FromInt(InventoryWidgetControllerRef->GetMaxInventoryWeight())));
}

void UTopBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ButtomPreviousCategory) ButtomPreviousCategory->OnClicked.AddDynamic(this, &UTopBarWidget::OnPrevClicked);
	if (ButtomNextCategory) ButtomNextCategory->OnClicked.AddDynamic(this, &UTopBarWidget::OnNextClicked);
}

void UTopBarWidget::HandleInventoryWeightChanged(float InInventoryWeight)
{
	float MaxInventoryWeight = InventoryWidgetControllerRef->GetMaxInventoryWeight();
	
	CurrentInventoryWeightText->SetText(FText::AsNumber(FMath::RoundToInt(InInventoryWeight)));
	
	if (bIsInventoryWeightGreaterThanAvailable && InInventoryWeight < MaxInventoryWeight)
	{
		CurrentInventoryWeightText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		bIsInventoryWeightGreaterThanAvailable = false;
		return;
	}
	
	if (InInventoryWeight > MaxInventoryWeight)
	{
		bIsInventoryWeightGreaterThanAvailable = true;
		CurrentInventoryWeightText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	}
}

void UTopBarWidget::OnPrevClicked()
{
	if (!OnSectionChanged.IsBound()) return;
	OnSectionChanged.Execute(-1);
}

void UTopBarWidget::OnNextClicked()
{
	if (!OnSectionChanged.IsBound()) return;
	OnSectionChanged.Execute(1);
}
