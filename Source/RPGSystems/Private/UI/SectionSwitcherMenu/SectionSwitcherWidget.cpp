// Fill out your copyright notice in the Description page of Project Settings.


#include "UI//SectionSwitcherMenu/SectionSwitcherWidget.h"

#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Game/PlayerController/RPGPlayerController.h"
#include "UI/Inventory/InventoryWidget.h"
#include "UI/SectionSwitcherMenu/InputContextWidget.h"
#include "UI/SectionSwitcherMenu/TopBarWidget.h"
#include "UI/WidgetController/InventoryWidgetController.h"

void USectionSwitcherWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SectionsCarousel = {EUISections::CHARACTERBUILD, EUISections::INVENTORY, EUISections::WORLDMAP, EUISections::MISSIONS };
	CurrentSectionIndex = 0;
	InitializeTopBarWidget();
}

void USectionSwitcherWidget::InitializeTopBarWidget()
{
	if (!IsValid(TopBarWidget)) return;
	TopBarWidget->SetInventoryWidgetController(GetInventoryWidgetController());
	TopBarWidget->InitCarousel(SectionsCarousel.Num());
	//Bind Delegates
	TopBarWidget->OnSectionChanged.BindUObject(this, &USectionSwitcherWidget::HandleSectionNavigation);
}


void USectionSwitcherWidget::SetPlayerControllerRef(ARPGPlayerController* PlayerController)
{
	PlayerControllerRef = PlayerController;
}

void USectionSwitcherWidget::ChangeSection(EUISections Section)
{
	CurrentSection = Section;
	//Change Section
	switch (Section)
	{
		case EUISections::INVENTORY:
			{
				if (!IsValid(InventoryWidgetRef))
				{
					UUserWidget* Widget = CreateWidget<UInventoryWidget>(PlayerControllerRef, InventoryWidgetClass);
					if (!IsValid(Widget)) return;
					InventoryWidgetRef = Cast<UInventoryWidget>(Widget);
					if (!IsValid(InventoryWidgetRef) || !IsValid(InventoryWidgetControllerRef)) return;
					
					InventoryWidgetRef->SetWidgetController(InventoryWidgetControllerRef);
					InventoryWidgetControllerRef->BroadCastInitialValues();
					
					SectionWidgetSwitcher->AddChild(InventoryWidgetRef);
				}

				if (SectionWidgetSwitcher->GetActiveWidget() != InventoryWidgetRef)
				{
					SectionWidgetSwitcher->SetActiveWidget(InventoryWidgetRef);
				}
				
				break;
			}
		case EUISections::CHARACTERBUILD: {  break; }
		case EUISections::WORLDMAP:       {  break; }
		case EUISections::MISSIONS:       {  break; }
		default: break;
	}

	//Input Context
	if (!IsValid(InputContextWidget)) return;
	InputContextWidget->SetKeyHints(Section);
}

UInventoryWidgetController* USectionSwitcherWidget::GetInventoryWidgetController()
{
	if (IsValid(InventoryWidgetControllerRef))
	{
		return InventoryWidgetControllerRef;
	}
	
	if (!IsValid(InventoryWidgetControllerClass))
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidgetControllerClass is NULL"));
		return nullptr;
	}
	
	InventoryWidgetControllerRef = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass);

	if (!IsValid(InventoryWidgetControllerRef))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create InventoryWidgetController"));
		return nullptr;
	}
		
	InventoryWidgetControllerRef->SetOwningActor(PlayerControllerRef);
	InventoryWidgetControllerRef->BindCallbacksToDependencies();
	InventoryWidgetControllerRef->BroadCastInitialValues();

	return InventoryWidgetControllerRef;
}

void USectionSwitcherWidget::HandleSectionNavigation(int32 Direction)
{
	int32 NumSections = SectionsCarousel.Num();
	int32 NewIndex = (CurrentSectionIndex + Direction + NumSections) % NumSections;
	
	CurrentSectionIndex = NewIndex;

	//Update Toip Bar Carousel
	if (!IsValid(TopBarWidget)) return;
	TopBarWidget->SetActiveSection(CurrentSectionIndex);
	
	EUISections PreviousSection = SectionsCarousel[CurrentSectionIndex - 1];
	EUISections NextSection = SectionsCarousel[CurrentSectionIndex];
	
	TopBarWidget->SetSectionData(EnumToString(PreviousSection),
		EnumToString(CurrentSection),EnumToString(NextSection));
	
	//Update Section Content
	ChangeSection(SectionsCarousel[NewIndex]);
}

FString USectionSwitcherWidget::EnumToString(EUISections Section)
{
	FString FullName = UEnum::GetValueAsString(Section);
	FString ShortName;
	FullName.Split(TEXT("::"), nullptr, &ShortName);
	return ShortName;
}


