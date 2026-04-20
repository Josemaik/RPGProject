// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerController/RPGPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Game/PlayerState/RPGPlayerState.h"
#include "Input/RPGInputConfig.h"
#include "Input/RPGSystemsInputComponent.h"
#include "InventorySection/InventoryComponent.h"
#include "UI/WidgetController/InventoryWidgetController.h"
#include "UI/Inventory/InventoryWidget.h"
#include "UI/SectionSwitcherMenu/SectionSwitcherWidget.h"


ARPGPlayerController::ARPGPlayerController()
{
	bReplicates = true;
	bEnableClickEvents = true;
}

UInventoryComponent* ARPGPlayerController::GetInventoryComponent_Implementation() const
{
	if (const ARPGPlayerState* PS = GetPlayerState<ARPGPlayerState>())
		return PS->InventoryComponent;

	return nullptr;
}

UEquipmentManagerComponent* ARPGPlayerController::GetEquipmentComponent_Implementation() const
{
	if (const ARPGPlayerState* PS = GetPlayerState<ARPGPlayerState>())
		return PS->EquipmentComponent;

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
	
	GetWorldTimerManager().SetTimerForNextTick(
	   this, &ARPGPlayerController::BindCallbacksToDependencies
   );
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
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"OnInventoryInput");
	if (InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Input.Inventory.Exit")))
	{
		DisableSectionWidget();
	}
}

void ARPGPlayerController::OnGameplayInput(FGameplayTag InputTag)
{
	if (!IsValid(SectionSwitcherWidget))
	{
		SectionSwitcherWidget = CreateWidget<USectionSwitcherWidget>(this, SectionSwitcherWidgetClass);
		if (!IsValid(SectionSwitcherWidget)) return;

		SectionSwitcherWidget->SetPlayerControllerRef(this,GetPlayerState<ARPGPlayerState>());
	}

	EnableSectionWidget();

	if (!SectionSwitcherWidget->IsInViewport())
	{
		SectionSwitcherWidget->AddToViewport();
	}
	
	if (InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Input.Inventory.Open")))
	{
		SectionSwitcherWidget->OpenSection(EUISections::INVENTORY);
	}
}

void ARPGPlayerController::BindCallbacksToDependencies()
{
	InventoryComponent = GetInventoryComponent();
	EquipmentComponent = GetEquipmentComponent();

	// ----------------------------
	// Inventory → Equipment binding
	// ----------------------------
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->EquipmentItemDelegate.AddUObject(
			this, &ARPGPlayerController::HandleEquipmentRequested
		);

		InventoryComponent->ItemDroppedDelegate.AddLambda(
			[this](const FRPGInventoryEntry* Entry,int32 NumItems)
		{
			SpawnDroppedItem(Entry, NumItems);
		});
	}

	// ----------------------------
	// Equipment → Inventory binding
	// ----------------------------
	if (IsValid(EquipmentComponent))
	{
		EquipmentComponent->EquipmentList.UnEquippedEntryDelegate.AddUObject(
		   this, &ARPGPlayerController::HandleUnEquippedItem);
	}
}

void ARPGPlayerController::HandleEquipmentRequested(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
	const FEquipmentEffectPackage& EffectPackage)
{
	if (IsValid(EquipmentComponent))
	{
		EquipmentComponent->EquipItem(EquipmentDefinition, EffectPackage);
	}
}

void ARPGPlayerController::HandleUnEquippedItem(const FRPGEquipmentEntry& UnEquippedEntry) 
{
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->AddUnEquippedItemEntry(UnEquippedEntry.EntryTag, UnEquippedEntry.EffectPackage);
	}
}

void ARPGPlayerController::SpawnDroppedItem(const FRPGInventoryEntry* DroppedEntry, int32 NumItems) const
{
	if (!DroppedEntry || !IsValid(InventoryComponent))
	{
		return;
	}
	
	TObjectPtr<APawn> OwnerPawn = GetPawn();
	if (!IsValid(OwnerPawn))
	{
		return;
	}
	
	const FVector FordwardLocation = OwnerPawn->GetActorLocation() + OwnerPawn->GetActorForwardVector() * ItemSpawnFordwardDistance;
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(FordwardLocation);

	InventoryComponent->SpawnItem(SpawnTransform, DroppedEntry, NumItems);
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
 	if (!IsValid(InventoryWidgetController))
 	{
 		InventoryWidgetController = NewObject<UInventoryWidgetController>(this, InventoryWidgetControllerClass); 
 		InventoryWidgetController->SetOwningActor(this);
 		InventoryWidgetController->BindCallbacksToDependencies();
 	}

	return InventoryWidgetController;
}

void ARPGPlayerController::EnableSectionWidget()
{
	if (!IsValid(SectionSwitcherWidget)) return;
	
	SectionSwitcherWidget->SetVisibility(ESlateVisibility::Visible);
	OverlayWidgetRef->SetVisibility(ESlateVisibility::Hidden);

	if (!IsValid(RPGAbilitySystemComponent))
	{
		return;
	}
 		
	RPGAbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Input.BlockInput.InventoryOpen")));
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
 		
	RPGAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Input.BlockInput.InventoryOpen")));

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	Subsystem->RemoveMappingContext(InventoryIMC);
	Subsystem->AddMappingContext(GameplayIMC, 1);

	SetInputMode(FInputModeGameOnly());
}


