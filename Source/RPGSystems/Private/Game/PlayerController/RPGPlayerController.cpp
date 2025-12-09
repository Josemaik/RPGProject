// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerController/RPGPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Game/PlayerState/RPGPlayerState.h"
#include "Input/RPGInputConfig.h"
#include "Input/RPGSystemsInputComponent.h"
#include "InventorySection/InventoryComponent.h"
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


