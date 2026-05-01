// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSortTypes.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "InventoryWidget.generated.h"

class UBaseCategoryWidget;
class UWidgetSwitcher;
class UItemToolTip;
class USortPanelWidget;
class UButton;
class UKeyHintWidget;
class UItemsDropToWorldWidget;
class UItemsPanelWidget;
class UEquipmentSlot;
class UItemCategoryButton;
class UUniformGridPanel;
struct FRPGInventoryEntry;
class UEditableText;
class UItemSlotWidget;
class UInventoryWidgetController;
class UInventoryComponent;
class UScrollBox;
class UTextBlock;
class UHorizontalBox;
class UWrapBox;
class UOverlay;
class USizeBox;
class UBorder;
class UCanvasPanel;
class UWidgetController;
class UVerticalBox;

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
	//void InitializeKeyHints();
	
	void SortItems(bool Quickly);

	UBaseCategoryWidget* GetActiveCategoryWidget() const;

protected:
	UFUNCTION()
	void OnQuickSortButtonClicked();

	UFUNCTION()
	void OnSortButtonClicked();

	UFUNCTION()
	void OnSortPanelCloseButtonClicked();

	UFUNCTION()
	void OnSortPanelOptionChanged(EItemSortType ItemSort);
	
	void NativeConstruct() override;
private:
	
	void HandleUnequipItem(const FRPGInventoryEntry& FrpgInventoryEntry);
	
	void InitEquipmentWidget(UEquipmentSlot* InEquipmentSlot);
	virtual void FinishDestroy() override;
	void OnEquipmentDropped(FGameplayTag ItemTag,uint64 ExistingID);
	//functions
	void BindInventoryItemDelegates();
	void HandleCategorySelected(FGameplayTag CategorySelected);
	
	//callbacks
	UFUNCTION()
	void HandleInventoryItemReceived(const FRPGInventoryEntry& Entry);

	UFUNCTION()
	void HandleItemDropped(const FRPGInventoryEntry& Entry) const;
	
	UFUNCTION()
	void HandleInventoryItemRemoved(const FRPGInventoryEntry& Entry) const;
	
	UFUNCTION()
	void OnSearchBarTextChanged(const FText& InText);

	UFUNCTION()
	void OnEquipItem(const FRPGInventoryEntry& Entry);

	void OnEquipKeyPressed();
	void OnDropKeyPressed();
	
	void SetSortPanelVisibility();
	void HideSortPanel();

	bool bShowSortPanelToggle = false;
	
	//References
	UPROPERTY(BlueprintReadOnly,meta=(allowPrivateAccess=true))
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;
	
	FGameplayTag CurrentCategorySelected;

	/////////////////////////////////
	//Categories Labels

	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* CategoriesContainer;

	UPROPERTY(EditAnywhere, Category="Categories")
	TArray<FCategoryButtonData> Categories;

	UPROPERTY(EditDefaultsOnly, Category="Categories")
	TSubclassOf<UItemCategoryButton> CategoryButtonClass;

	//Search Bar
	// UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	// UEditableText* SearchBar;

	// UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	// UTextBlock* CategoryText;
	
	/////////////////////////////////
	//ItemsPanel

	// UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	// UItemsPanelWidget* ItemsContainer;
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* CategorySwitcher;

	UPROPERTY(EditDefaultsOnly, Category="Categories")
	TMap<FGameplayTag, TSubclassOf<UBaseCategoryWidget>> CategoryWidgetClasses;

	UPROPERTY()
	TMap<FGameplayTag, UBaseCategoryWidget*> CategoryWidgets;

	///Sort Panel
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	TObjectPtr<USortPanelWidget> SortPanelWidget;

	//Sort items buttons
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UButton* SortButton;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UButton* QuickSortButton;

	/////////////////////////////////
	//Drop To World Panel
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UItemsDropToWorldWidget* ItemsDropToWorldWidget;

	UPROPERTY()
	UItemToolTip* ItemToolTipReference;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UItemToolTip> TooltipWidgetClass;
	
	//Equipments Panel
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UEquipmentSlot* SilverSword;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UEquipmentSlot* SteelWeapon;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UEquipmentSlot* Bolls;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UEquipmentSlot* RangedWeapon;

	TArray<UEquipmentSlot*> EquipmentSlots;
};



