// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "RPGSystemsWidget.generated.h"

class UCategoryButton;
class UUniformGridPanel;
struct FRPGInventoryEntry;
class UEditableText;
class UItemRowWidget;
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
class RPGSYSTEMS_API URPGSystemsWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetWidgetController(UWidgetController* InWidgetController);
protected:
	void NativeConstruct() override;
private:
	virtual void FinishDestroy() override;
	//functions
	void CacheEssentialVars();
	void BindInventoryItemDelegates();
	void ClearEntries();
	void HandleCategorySelected(FGameplayTag CategorySelected);
	void AddToItemsGrid(TObjectPtr<UItemRowWidget> ItemSlot);

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
	
	UPROPERTY(BlueprintReadOnly, meta=(allowPrivateAccess=true))
	TObjectPtr<UWidgetController> WidgetController;

	UPROPERTY(BlueprintReadOnly,meta=(allowPrivateAccess=true))
	TObjectPtr<UInventoryWidgetController> InventoryWidgetController;

	UPROPERTY()
	UInventoryComponent* OwningInventory;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemRowWidget> ItemSlotWidget;
	
	UPROPERTY()
	TObjectPtr<UItemRowWidget> CurrentItemSlotWidget;
	
	//TArray<UItemRowWidget*> ActiveItemRowWidgets;
	TMap<int64, UItemRowWidget*> ActiveItemSlotWidgets; //created item slots
	int32 CurrentCategoryIndex = 0;

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
	TSubclassOf<UCategoryButton> CategoryButtonClass;

	/////////////////////////////////
	//ItemsPanel
	UPROPERTY(VisibleAnywhere, Category = "UI")
	UBorder* BackgroundBorder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess), Category = "UI")
	USizeBox* SizeBox;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UOverlay* CoreOverlay; 

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UWrapBox* WrapBox;

	// UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	// UHorizontalBox* HorizontalBox;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UEditableText* SearchBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"), Category = "UI")
	UScrollBox* InventoryContent;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UUniformGridPanel* ItemsPanel;
	
	// UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	// UTextBlock* ItemDescriptionText;
	
	FGameplayTag CurrentCategorySelected;
	
	const int32 MaxColumns = 5;
};


