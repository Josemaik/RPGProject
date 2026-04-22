// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerController/RPGPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/RPGSystemsCharacter.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Game/PlayerState/RPGPlayerState.h"
#include "Input/RPGInputConfig.h"
#include "Input/RPGSystemsInputComponent.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/WidgetController/InventoryWidgetController.h"
#include "UI/SectionSwitcherMenu/SectionSwitcherWidget.h"
#include "AbilitySystem/NativeTags/RPGInputTags.h"


ARPGPlayerController::ARPGPlayerController()
{
	bReplicates = true;
	bEnableClickEvents = true;
}

UInventoryComponent* ARPGPlayerController::GetInventoryComponent_Implementation() const
{
	if (const ARPGSystemsCharacter* RPGCharacter = Cast<ARPGSystemsCharacter>(GetCharacter()))
		return RPGCharacter->GetInventoryComponent();

	return nullptr;
}

UEquipmentManagerComponent* ARPGPlayerController::GetEquipmentComponent_Implementation() const
{
	if (const ARPGSystemsCharacter* RPGCharacter = Cast<ARPGSystemsCharacter>(GetCharacter()))
		return RPGCharacter->GetEquipmentComponent();

	return nullptr;
}

void ARPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsValid(RPGInputConfig))
	{
		return;
	}

	URPGSystemsInputComponent* RPGInputComp = Cast<URPGSystemsInputComponent>(InputComponent);
	if (!IsValid(RPGInputComp))
	{
		return;
	}

	RPGInputComp->BindAbilityActions(RPGInputConfig, this, &ThisClass::AbilityInputPressed, &ThisClass::AbilityInputReleased);
	RPGInputComp->BindInventoryActions(RPGInputConfig,this, &ThisClass::OnInventoryInput);
	RPGInputComp->BindGameplayActions(RPGInputConfig,this, &ThisClass::OnGameplayInput);
}

void ARPGPlayerController::InitPlayerState()
{
	Super::InitPlayerState();

	if (ARPGPlayerState* RPGPlayerState = Cast<ARPGPlayerState>(PlayerState))
	{
		RPGAbilitySystemComponent = RPGPlayerState->GetRPGAbilitySystemComponent();
	}
}

void ARPGPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (ARPGPlayerState* RPGPlayerState = Cast<ARPGPlayerState>(PlayerState))
	{
		RPGAbilitySystemComponent = RPGPlayerState->GetRPGAbilitySystemComponent();
	}
}

void ARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Create Section Switcher Widget Instance
	SectionSwitcherWidget = CreateWidget<USectionSwitcherWidget>(this, SectionSwitcherWidgetClass);
	SectionSwitcherWidget->SetPlayerControllerRef(this,GetPlayerState<ARPGPlayerState>());
}

void ARPGPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
	if (!IsValid(RPGAbilitySystemComponent))
	{
		return;
	}

	RPGAbilitySystemComponent->AbilityInputPressed(InputTag);
}

void ARPGPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
	if (!IsValid(RPGAbilitySystemComponent))
	{
		return;
	}

	RPGAbilitySystemComponent->AbilityInputReleased(InputTag);
}

void ARPGPlayerController::OnInventoryInput(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(RPGInputTags::Inventory::Exit))
	{
		DisableSectionWidget();
	}
	if (InputTag.MatchesTagExact(RPGInputTags::Inventory::SortItemsQuickly))
	{
		GetInventoryWidgetController()->RequestSortItems(true);
	}
	if (InputTag.MatchesTagExact(RPGInputTags::Inventory::SortItems))
	{
		GetInventoryWidgetController()->RequestSortItems(false);
	}
}

void ARPGPlayerController::OnGameplayInput(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(RPGInputTags::Inventory::Open))
	{
		EnableSectionWidget();
		SectionSwitcherWidget->OpenSection(EUISections::INVENTORY);
	}
}


UAbilitySystemComponent* ARPGPlayerController::GetAbilitySystemComponent() const
{
	return RPGAbilitySystemComponent;
}

void ARPGPlayerController::SetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag,int32 AbilityLevel)
{
	if (!IsValid(RPGAbilitySystemComponent))
	{
		return;
	}

	RPGAbilitySystemComponent->SetDynamicProjectile(ProjectileTag,AbilityLevel);
}

UInventoryWidgetController* ARPGPlayerController::GetInventoryWidgetController()
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
		
	InventoryWidgetControllerRef->SetOwningActor(this);
	InventoryWidgetControllerRef->BindCallbacksToDependencies();
	InventoryWidgetControllerRef->BroadCastInitialValues();

	return InventoryWidgetControllerRef;
}

void ARPGPlayerController::EnableSectionWidget()
{
	if (!IsValid(SectionSwitcherWidget)) return;

	if (!SectionSwitcherWidget->IsInViewport())
	{
		SectionSwitcherWidget->AddToViewport();
	}
	
	SectionSwitcherWidget->SetVisibility(ESlateVisibility::Visible);
	OverlayWidgetRef->SetVisibility(ESlateVisibility::Hidden);

	if (!IsValid(RPGAbilitySystemComponent))
	{
		return;
	}
 		
	RPGAbilitySystemComponent->AddLooseGameplayTag(RPGInputTags::BlockInput::InventoryIsOpen);
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	Subsystem->RemoveMappingContext(GameplayIMC);
	Subsystem->AddMappingContext(InventoryIMC, 1);

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void ARPGPlayerController::DisableSectionWidget()
{
	if (!IsValid(SectionSwitcherWidget)) return;

	SectionSwitcherWidget->SetVisibility(ESlateVisibility::Collapsed);
	OverlayWidgetRef->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (!IsValid(RPGAbilitySystemComponent))
	{
		return;
	}
 		
	RPGAbilitySystemComponent->RemoveLooseGameplayTag(RPGInputTags::BlockInput::InventoryIsOpen);

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	Subsystem->RemoveMappingContext(InventoryIMC);
	Subsystem->AddMappingContext(GameplayIMC, 1);

	SetInputMode(FInputModeGameOnly());
}


