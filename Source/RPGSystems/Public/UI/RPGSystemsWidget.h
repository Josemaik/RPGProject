// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "RPGSystemsWidget.generated.h"

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

	//callbacks
	UFUNCTION()
	void HandleInventoyItemReceived(const FRPGInventoryEntry& Entry);

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
	TSubclassOf<UItemRowWidget> ItemRowWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UItemRowWidget> CurrentItemRowWidget;
	
	//TArray<UItemRowWidget*> ActiveItemRowWidgets;
	TMap<int64, UItemRowWidget*> ActiveItemRowWidgets;

	//Hierarchy
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UCanvasPanel* CanvasPanel;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UBorder* BackgroundBorder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess), Category = "UI")
	USizeBox* SizeBox;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UOverlay* CoreOverlay; 

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UWrapBox* WrapBox;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UHorizontalBox* HorizontalBox;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UTextBlock* InventoryText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UEditableText* SearchBar;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"), Category = "UI")
	UScrollBox* InventoryContent;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UTextBlock* ItemDescriptionText;
};


