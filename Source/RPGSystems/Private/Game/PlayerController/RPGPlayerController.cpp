// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerController/RPGPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
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

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->SetIsReplicated(true);
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

// UAbilitySystemComponent* ARPGPlayerController::GetAbilitySystemComponent() const
// {
// 	
// }

void ARPGPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGPlayerController, InventoryComponent);
}

void ARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InventoryComponent))
	{
		InventoryComponent->SetOwnerLocallyControlled(IsLocalController());
	}
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

UInventoryComponent* ARPGPlayerController::GetInventoryComponent_Implementation()
{
	return InventoryComponent;
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


