// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PlayerController/RPGPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
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

void ARPGPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGPlayerController, InventoryComponent);
}

UInventoryComponent* ARPGPlayerController::GetInventoryComponent_Implementation()
{
	return InventoryComponent;
}

UAbilitySystemComponent* ARPGPlayerController::GetAbilitySystemComponent() const
{
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
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


