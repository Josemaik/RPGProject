// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "InventoryWidget.generated.h"

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
protected:
	void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
private:
	virtual void FinishDestroy() override;
	//functions
	void CacheEssentialVars();
	void BindInventoryItemDelegates();
	void ClearEntries();
	void HandleCategorySelected(FGameplayTag CategorySelected);
	void AddToItemsGrid(TObjectPtr<UItemSlotWidget> InSlotWidget);

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
	void HandleInventoryItemRemoved(const int64 ItemID);

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
	UTextBlock* InventoryText;

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
	
	// UPROPERTY(VisibleAnywhere, Category = "UI")
	// UBorder* BackgroundBorder;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess), Category = "UI")
	// USizeBox* SizeBox;
	
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"), Category = "UI")
	// UScrollBox* InventoryContent;
	//
	// UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	// UUniformGridPanel* ItemsPanel;
	//
	// const int32 MaxColumns = 5;

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



