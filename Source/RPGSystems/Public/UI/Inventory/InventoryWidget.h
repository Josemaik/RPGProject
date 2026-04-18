// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "InventoryWidget.generated.h"

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
	void InitializeKeyHints();

	void SortItems() const;
protected:
	void NativeConstruct() override;
private:
	virtual void FinishDestroy() override;
	//functions
	void CacheEssentialVars();
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
	void HandleItemDropped(const FRPGInventoryEntry& Entry);
	
	UFUNCTION()
	void HandleInventoryItemRemoved(const FRPGInventoryEntry& Entry) const;

	UFUNCTION()
	void HandleInventoryWeight(const float Weight);
	
	UFUNCTION()
	void OnSearchBarTextChanged(const FText& InText);

	UFUNCTION()
	void OnEquipItem(const FRPGInventoryEntry& Entry);
	
	//References
	UPROPERTY(BlueprintReadOnly, meta=(allowPrivateAccess=true))
	TObjectPtr<UWidgetController> WidgetController;

	UPROPERTY(BlueprintReadOnly,meta=(allowPrivateAccess=true))
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY()
	UInventoryComponent* OwningInventory;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;
	
	FGameplayTag CurrentCategorySelected;

	//Hierarchy
	UPROPERTY(VisibleAnywhere, Category = "UI")
	UCanvasPanel* CanvasPanel;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	UTextBlock* MaxInventoryWeight;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = "UI")
	UTextBlock* CurrentInventoryWeightText;

	bool WeightIsHiguerThanTheMaxAvailable = false;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = "UI")
	UTextBlock* MaxInventoryWeightText;

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

	//Key Hints
	UPROPERTY(meta = (BindWidget))
	UKeyHintWidget* KeyHintWidget_SortItems;

	UPROPERTY(meta = (BindWidget))
	UKeyHintWidget* KeyHintWidget_DropItem;

	UPROPERTY(meta = (BindWidget))
	UKeyHintWidget* KeyHintWidget_EquipItem;
};



