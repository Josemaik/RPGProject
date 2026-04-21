 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/InventoryInterface.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "RPGPlayerController.generated.h"

 class USectionSwitcherWidget;
 class UInputMappingContext;
 struct FRPGInventoryEntry;
 struct FRPGEquipmentEntry;
 struct FEquipmentEffectPackage;
 class UEquipmentDefinition;
 class ARPGPlayerState;
class UEquipmentManagerComponent;
 class URPGAbilitySystemComponent;
 class URPGInputConfig;
 class UInventoryWidget;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API ARPGPlayerController : public APlayerController, public IAbilitySystemInterface, public IInventoryInterface, public IRPGAbilitySystemInterface
{
	GENERATED_BODY()
public:

	ARPGPlayerController();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Inventory")
	UInventoryComponent* GetInventoryComponent() const;
	/*Implements inventory interface*/
	virtual UInventoryComponent* GetInventoryComponent_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Equipment")
	UEquipmentManagerComponent* GetEquipmentComponent() const;
	virtual UEquipmentManagerComponent* GetEquipmentComponent_Implementation() const override;
	
	virtual void SetupInputComponent() override;
	virtual void InitPlayerState() override;
	virtual void OnRep_PlayerState() override;
	
	/*Implements ability system interface*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/*Implements rpg ability system interface*/
	virtual void SetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag,int32 AbilityLevel) override;
	
	UInventoryWidgetController* GetInventoryWidgetController();
	
	UFUNCTION(BlueprintCallable)
	void EnableSectionWidget();

	UFUNCTION(BlueprintCallable)
	void DisableSectionWidget();
	
protected:
	virtual void BeginPlay() override;
	
	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);

	void OnInventoryInput(FGameplayTag InputTag);
	void OnGameplayInput(FGameplayTag InputTag);
private:

	bool bToggleSectionSwitcherWidget = false;

	//Controllers
	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetControllerRef;
	
	//Components
	// UPROPERTY()
	// UInventoryComponent* InventoryComponent;
	//
	// UPROPERTY()
	// UEquipmentManagerComponent* EquipmentComponent;
	
	UPROPERTY()
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComponent;

	//Input
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Input")
	TObjectPtr<URPGInputConfig> RPGInputConfig;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Input")
	TObjectPtr<UInputMappingContext> GameplayIMC;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Input")
	TObjectPtr<UInputMappingContext> InventoryIMC;

	//Widgets
	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<USectionSwitcherWidget> SectionSwitcherWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<USectionSwitcherWidget> SectionSwitcherWidgetClass;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUserWidget> OverlayWidgetRef;
};


 
