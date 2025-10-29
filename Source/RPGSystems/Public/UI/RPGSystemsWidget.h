// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGSystemsWidget.generated.h"

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

private:
	virtual void FinishDestroy() override;
	//functions
	void CacheEssentialVars();
	void BindInventoryItemDelegates();

	//callbacks
	UFUNCTION()
	void HandleInventoyItemReceived(const FMasterItemDefinition& Master);

	UFUNCTION()
	void OnInventoryItemBroadcastComplete();

	UFUNCTION()
	void OnScrollBoxReset();
	
	
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
	
	TArray<UItemRowWidget*> ActiveItemRowWidgets;

	//Hierarchy
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UCanvasPanel* CanvasPanel;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UBorder* BackgroundBorder;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	USizeBox* SizeBox;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UOverlay* CoreOverlay; 

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UWrapBox* WrapBox;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UHorizontalBox* HorizontalBox;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UTextBlock* InventoryText;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"), Category = "UI")
	UScrollBox* InventoryContent;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UTextBlock* ItemDescriptionText;
};
