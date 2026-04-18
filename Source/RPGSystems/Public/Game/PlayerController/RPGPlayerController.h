 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/InventoryInterface.h"
#include "Interfaces/RPGAbilitySystemInterface.h"
#include "RPGPlayerController.generated.h"

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
	virtual UEquipmentManagerComponent* GetEquipmentComponent_Implementation() const;
	
	virtual void SetupInputComponent() override;
	virtual void InitPlayerState() override;
	virtual void OnRep_PlayerState() override;
	
	/*Implements ability system interface*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/*Implements rpg ability system interface*/
	virtual void SetDynamicProjectile_Implementation(const FGameplayTag& ProjectileTag,int32 AbilityLevel) override;
	
	UInventoryWidgetController* GetInventoryWidgetController();

	UFUNCTION(BlueprintCallable)
	void CreateInventoryWidget();

	UFUNCTION(BlueprintCallable)
	void EnableInventoryWidget();

	UFUNCTION(BlueprintCallable)
	void DisableInventoryWidget();
	
protected:
	virtual void BeginPlay() override;
	
	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);

	void OnInventoryInput(FGameplayTag InputTag);
private:

	void BindCallbacksToDependencies();

	UFUNCTION()
	void HandleEquipmentRequested(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
				   const FEquipmentEffectPackage& EffectPackage);

	UFUNCTION()
	void HandleUnEquippedItem(const FRPGEquipmentEntry& UnEquippedEntry);
	
	void SpawnDroppedItem(const FRPGInventoryEntry* DroppedEntry, int32 NumItems) const;
	
	bool bCallbacksbound = false;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Item Spawn")
	float ItemSpawnFordwardDistance = 250.f;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	UEquipmentManagerComponent* EquipmentComponent;
	
	UPROPERTY()
	TObjectPtr<URPGAbilitySystemComponent> RPGAbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Input")
	TObjectPtr<URPGInputConfig> RPGInputConfig;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Input")
	TObjectPtr<UInputMappingContext> GameplayIMC;

	UPROPERTY(EditDefaultsOnly, Category="Custom Values|Input")
	TObjectPtr<UInputMappingContext> InventoryIMC;
	
	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUserWidget> OverlayWidgetRef;
};


 
