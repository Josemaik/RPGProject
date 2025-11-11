// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerController/RPGPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/ActorChannel.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Game/PlayerState/RPGPlayerState.h"
#include "Input/RPGInputConfig.h"
#include "Input/RPGSystemsInputComponent.h"
#include "InventorySection/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/WidgetController/InventoryWidgetController.h"
#include "UI/RPGSystemsWidget.h"


ARPGPlayerController::ARPGPlayerController()
{
	bReplicates = true;
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
}

void ARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimerForNextTick(this, &ARPGPlayerController::BindCallbacksToDependencies);
}

void ARPGPlayerController::AbilityInputPressed(FGameplayTag InputTag)
{
	if (!IsValid(GetRPGAbilitySystemComponent()))
	{
		return;
	}

	RPGAbilitySystemComponent->AbilityInputPressed(InputTag);
}

void ARPGPlayerController::AbilityInputReleased(FGameplayTag InputTag)
{
	if (!IsValid(GetRPGAbilitySystemComponent()))
	{
		return;
	}

	RPGAbilitySystemComponent->AbilityInputReleased(InputTag);
}

URPGAbilitySystemComponent* ARPGPlayerController::GetRPGAbilitySystemComponent()
{
	if (!IsValid(RPGAbilitySystemComponent))
	{
		if (const ARPGPlayerState* RPGPlayerState = GetPlayerState<ARPGPlayerState>())
		{
			RPGAbilitySystemComponent = RPGPlayerState->GetRPGAbilitySystemComponent();
		}
	}

	return RPGAbilitySystemComponent;
}

void ARPGPlayerController::BindCallbacksToDependencies()
{
	UInventoryComponent* InvComp = GetInventoryComponent();
	UEquipmentManagerComponent* EquipComp = GetEquipmentComponent();

	// ----------------------------
	// Inventory → Equipment binding
	// ----------------------------
	if (IsValid(InvComp))
	{
		InvComp->EquipmentItemDelegate.AddLambda(
			[this](const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
				   const FEquipmentEffectPackage& EffectPackage)
			{
				if (UEquipmentManagerComponent* Equip = GetEquipmentComponent())
				{
					Equip->EquipItem(EquipmentDefinition, EffectPackage);
				}
			});
	}

	// ----------------------------
	// Equipment → Inventory binding
	// ----------------------------
	if (IsValid(EquipComp))
	{
		EquipComp->EquipmentList.UnEquippedEntryDelegate.AddLambda(
			[this](const FRPGEquipmentEntry& UnEquippedEntry)
			{
				if (UInventoryComponent* Inv = GetInventoryComponent())
				{
					Inv->AddUnEquippedItemEntry(UnEquippedEntry.EntryTag, UnEquippedEntry.EffectPackage);
				}
			});
	}
}

UAbilitySystemComponent* ARPGPlayerController::GetAbilitySystemComponent() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
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

void ARPGPlayerController::CreateInventoryWidget()
{
 	if (UUserWidget* Widget = CreateWidget<URPGSystemsWidget>(this, InventoryWidgetClass))
 	{
 		InventoryWidget = Cast<URPGSystemsWidget>(Widget);
 		InventoryWidget->SetWidgetController(GetInventoryWidgetController());
 		InventoryWidgetController->BroadCastInitialValues();
 		InventoryWidget->AddToViewport();
 	}
}


