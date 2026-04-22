// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "InventoryWidget.generated.h"

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

protected:
	UFUNCTION()
	void OnQuickSortButtonClicked();

	UFUNCTION()
	void OnSortButtonClicked();
	
	void NativeConstruct() override;
private:
	virtual void FinishDestroy() override;
	//functions
	void BindInventoryItemDelegates();
	void HandleCategorySelected(FGameplayTag CategorySelected);

	UItemSlotWidget* NewActiveItem(const FRPGInventoryEntry& Entry);
	void BindItemSlotDelegates(UItemSlotWidget* CurrentItemSlotWidget);
	void AddItemToGrid(const FRPGInventoryEntry& Entry, const FMasterItemDefinition& ItemDefinition);
	
	//callbacks
	UFUNCTION()
	void HandleInventoryItemReceived(const FRPGInventoryEntry& Entry);

	UFUNCTION()
	void HandleItemRowClicked(const FRPGInventoryEntry& Entry);

	UFUNCTION()
	void HandleItemDropped(const FRPGInventoryEntry& Entry) const;
	
	UFUNCTION()
	void HandleInventoryItemRemoved(const FRPGInventoryEntry& Entry) const;
	
	UFUNCTION()
	void OnSearchBarTextChanged(const FText& InText);

	UFUNCTION()
	void OnEquipItem(const FRPGInventoryEntry& Entry);
	
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
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UEditableText* SearchBar;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UTextBlock* CategoryText;
	
	/////////////////////////////////
	//ItemsPanel

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UItemsPanelWidget* ItemsContainer;

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

	//Equipments Panel
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UEquipmentSlot* SilverSword;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UEquipmentSlot* SteelWeapon;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UEquipmentSlot* Bolls;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UEquipmentSlot* RangedWeapon;
};



