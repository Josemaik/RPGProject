// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/ItemSortTypes.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "InventoryWidget.generated.h"

class UHorizontalBox;
class UItemSlotWidget;
class UBaseCategoryWidget;
class UWidgetSwitcher;
class UItemToolTip;
class USortPanelWidget;
class UButton;
class UItemsDropToWorldWidget;
class UEquipmentSlot;
class UItemCategoryButton;
struct FRPGInventoryEntry;
class UInventoryWidgetController;

USTRUCT(BlueprintType)
struct FCategoryButtonData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag CategoryTag;

	UPROPERTY(EditAnywhere)
	FText DisplayName;

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;
};

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetWidgetController(UInventoryWidgetController* InWidgetController);
	
	void SortItems(bool Quickly);

protected:
	virtual void NativeConstruct() override;
	virtual void FinishDestroy() override;
private:
	
	void HandleUnequipItem(const FRPGInventoryEntry& FrpgInventoryEntry);
	void InitEquipmentWidget(UEquipmentSlot* InEquipmentSlot);
	void OnEquipmentDropped(FGameplayTag ItemTag,uint64 ExistingID);
	void BindInventoryItemDelegates();
	void HandleCategorySelected(FGameplayTag CategorySelected);
	
	//callbacks
	UFUNCTION()
	void HandleInventoryItemReceived(const FRPGInventoryEntry& Entry);

	UFUNCTION()
	void HandleItemDropped(const FRPGInventoryEntry& Entry) const;
	
	UFUNCTION()
	void HandleInventoryItemRemoved(const FRPGInventoryEntry& Entry);

	UFUNCTION()
	void OnEquipItem(const FRPGInventoryEntry& Entry);

	UFUNCTION()
	void OnQuickSortButtonClicked();

	UFUNCTION()
	void OnSortButtonClicked();

	UFUNCTION()
	void OnSortPanelCloseButtonClicked();

	UFUNCTION()
	void OnSortPanelOptionChanged(EItemSortType ItemSort);

	void OnEquipKeyPressed();
	void OnDropKeyPressed();

	//Sort Panel
	void SetSortPanelVisibility();
	void HideSortPanel();
	
	bool bShowSortPanelToggle = false;
	FGameplayTag CurrentCategorySelected;
	
	TMap<FGameplayTag, UItemCategoryButton*> CategoryButtonsMap;
	
	UPROPERTY(EditAnywhere, Category="Categories")
	TArray<FCategoryButtonData> Categories;

	UPROPERTY()
	TMap<FGameplayTag, UBaseCategoryWidget*> CategoryWidgets;

	TArray<UEquipmentSlot*> EquipmentSlots;
	
	//References
	UPROPERTY(BlueprintReadOnly,meta=(allowPrivateAccess=true))
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY()
	UItemCategoryButton* LastCategorySelected;

	UPROPERTY()
	UItemToolTip* ItemToolTipReference;

	//Subclasses
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Categories")
	TSubclassOf<UItemCategoryButton> CategoryButtonClass;

	UPROPERTY(EditDefaultsOnly, Category="Categories")
	TMap<FGameplayTag, TSubclassOf<UBaseCategoryWidget>> CategoryWidgetClasses;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UItemToolTip> TooltipWidgetClass;

	///////////////////
	// Layout
	
	//Categories Labels

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> CategoriesContainer;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> CategorySwitcher;

	///Sort Panel
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	TObjectPtr<USortPanelWidget> SortPanelWidget;

	//Sort items buttons
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	TObjectPtr<UButton> SortButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	TObjectPtr<UButton> QuickSortButton;

	/////////////////////////////////
	//Drop To World Panel
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	TObjectPtr<UItemsDropToWorldWidget> ItemsDropToWorldWidget;
	
	//Equipments Panel
	//Weapons
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> SilverSword;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> SteelWeapon;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> Bolls;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> RangedWeapon;
	
	//Consumables
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> ConsumableSlot0;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> ConsumableSlot1;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> ConsumableSlot2;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> ConsumableSlot3;
	
	//Armor
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> ChestSlot;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> GauntletSlot;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> BootsSlot;
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	TObjectPtr<UEquipmentSlot> TrousersSLot;
};



