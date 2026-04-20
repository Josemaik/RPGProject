// Fill out your copyright notice in the Description page of Project Settings.


#include "UI//SectionSwitcherMenu/SectionSwitcherWidget.h"

#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Game/PlayerController/RPGPlayerController.h"
#include "UI/CharacterBuild/CharacterBuildWidget.h"
#include "UI/Inventory/InventoryWidget.h"
#include "UI/Map/WorldMapWidget.h"
#include "UI/Missions/MissionsWidget.h"
#include "UI/SectionSwitcherMenu/InputContextWidget.h"
#include "UI/SectionSwitcherMenu/TopBarWidget.h"
#include "UI/WidgetController/InventoryWidgetController.h"

void USectionSwitcherWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SectionsCarousel = {EUISections::CHARACTERBUILD, EUISections::INVENTORY, EUISections::WORLDMAP, EUISections::MISSIONS };
	CurrentSectionIndex = 0;
	CurrentSection = EUISections::NONE;
}

void USectionSwitcherWidget::InitializeTopBarWidget()
{
	if (!IsValid(TopBarWidget)) return;
	TopBarWidget->SetInventoryWidgetController(GetInventoryWidgetController());
	TopBarWidget->InitCarousel(SectionsCarousel.Num());
	TopBarWidget->UpdateExperience(1, 0, CachedPlayerState->GetRequiredExperience());

	//Bind Delegates
	TopBarWidget->OnSectionChanged.BindUObject(this, &USectionSwitcherWidget::HandleSectionNavigation);
	CachedPlayerState->OnExperienceChangedDelegate.AddLambda([this](int32 PlayerLevel, int32 NewExperience, int32 RequiredExperience){
		TopBarWidget->UpdateExperience(PlayerLevel, NewExperience, RequiredExperience);
	});
}


void USectionSwitcherWidget::SetPlayerControllerRef(ARPGPlayerController* PlayerController,ARPGPlayerState* PlayerState)
{
	PlayerControllerRef = PlayerController;
	CachedPlayerState = PlayerState;

	InitializeTopBarWidget();
}

void USectionSwitcherWidget::OpenSection(EUISections Section)
{
	if (CurrentSection == Section) return;
	int32 Index = SectionsCarousel.Find(Section);
	CurrentSectionIndex = Index;
	ChangeSection(Section);
}

void USectionSwitcherWidget::ChangeSection(EUISections Section)
{
	CurrentSection = Section;
	//Update Carousel Data
	TopBarWidget->SetActiveSection(CurrentSectionIndex);

	int32 PreviousIndex = CurrentSectionIndex - 1 < 0 ? SectionsCarousel.Num() - 1 : CurrentSectionIndex - 1;
	int32 NextIndex = CurrentSectionIndex + 1 == SectionsCarousel.Num() ? 0 : CurrentSectionIndex + 1;
	EUISections PreviousSection = SectionsCarousel[PreviousIndex];
	EUISections NextSection = SectionsCarousel[NextIndex];
	
	TopBarWidget->SetSectionData(EnumToString(PreviousSection),EnumToString(Section),EnumToString(NextSection));
	
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
		case EUISections::CHARACTERBUILD:
			{
				if (!IsValid(CharacterBuildWidgetRef))
				{
					UUserWidget* Widget = CreateWidget<UCharacterBuildWidget>(PlayerControllerRef, CharacterBuildWidgetClass);
					if (!IsValid(Widget)) return;
					CharacterBuildWidgetRef = Cast<UCharacterBuildWidget>(Widget);
					if (!IsValid(CharacterBuildWidgetRef)) return;
					
					SectionWidgetSwitcher->AddChild(CharacterBuildWidgetRef);
				}

				if (SectionWidgetSwitcher->GetActiveWidget() != CharacterBuildWidgetRef)
				{
					SectionWidgetSwitcher->SetActiveWidget(CharacterBuildWidgetRef);
				}
				
				break;
			}
		case EUISections::WORLDMAP:
			{
				if (!IsValid(WorldMapWidgetRef))
				{
					UUserWidget* Widget = CreateWidget<UWorldMapWidget>(PlayerControllerRef, WorldMapWidgetClass);
					if (!IsValid(Widget)) return;
					WorldMapWidgetRef = Cast<UWorldMapWidget>(Widget);
					if (!IsValid(WorldMapWidgetRef)) return;
					
					SectionWidgetSwitcher->AddChild(WorldMapWidgetRef);
				}

				if (SectionWidgetSwitcher->GetActiveWidget() != WorldMapWidgetRef)
				{
					SectionWidgetSwitcher->SetActiveWidget(WorldMapWidgetRef);
				}
				break;
			}
		case EUISections::MISSIONS:
			{
				if (!IsValid(MissionsWidgetRef))
				{
					UUserWidget* Widget = CreateWidget<UMissionsWidget>(PlayerControllerRef, MissionsdWidgetClass);
					if (!IsValid(Widget)) return;
					MissionsWidgetRef = Cast<UMissionsWidget>(Widget);
					if (!IsValid(MissionsWidgetRef)) return;
					
					SectionWidgetSwitcher->AddChild(MissionsWidgetRef);
				}

				if (SectionWidgetSwitcher->GetActiveWidget() != MissionsWidgetRef)
				{
					SectionWidgetSwitcher->SetActiveWidget(MissionsWidgetRef);
				}
				
				break;
			}
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
	CurrentSectionIndex = (CurrentSectionIndex + Direction + NumSections) % NumSections;
	
	//Update Section Content
	ChangeSection(SectionsCarousel[CurrentSectionIndex]);
}

FString USectionSwitcherWidget::EnumToString(EUISections Section)
{
	FString FullName = UEnum::GetValueAsString(Section);
	FString ShortName;
	FullName.Split(TEXT("::"), nullptr, &ShortName);
	return ShortName;
}


