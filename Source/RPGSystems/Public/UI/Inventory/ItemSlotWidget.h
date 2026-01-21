// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "ItemSlotWidget.generated.h"

class UUniformGridSlot;
class UInventoryWidgetController;
class USizeBox;
class UItemSlotIcon;
struct FRPGInventoryEntry;
struct FMasterItemDefinition;
class UTextBlock;
class UButton;


DECLARE_DELEGATE_OneParam(FOnUseButtomClicked, const FRPGInventoryEntry&);
DECLARE_DELEGATE_OneParam(FOnItemRowClicked, const FRPGInventoryEntry&);
DECLARE_DELEGATE_OneParam(FOnItemDroppedEvent, const FRPGInventoryEntry&);
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnUseButtomClicked	OnUseButtomClickedDelegate;
	FOnItemRowClicked OnItemRowClickedDelegate;
	FOnItemDroppedEvent OnItemDroppedEventDelegate;
	
	void SetItemNameText(FText Text);
	void SetQuantityText(int32 Quantity);
	void SetIcon();
	void SetGridSlot(const int32 Index,UUniformGridSlot* GridSlot);
	
	void Init(const FRPGInventoryEntry& InItemDefinition,TSoftObjectPtr<UTexture2D> Icon,TObjectPtr<UInventoryWidgetController> InventoryWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	FRPGInventoryEntry ItemEntry;
	
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
private:
	
	UFUNCTION()
	void OnItemUpdated(const FRPGInventoryEntry& UpdatedEntry);

	//References
	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> WidgetController;
	
	UPROPERTY()
	TObjectPtr<UUniformGridSlot> GridSlot = nullptr;
	
	int32 GridIndex = 0;
	
	UPROPERTY()
	TSoftObjectPtr<UTexture2D> SoftIconTexture;
	
	UPROPERTY()
	UItemSlotIcon* IconWidgetReference;
	
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<UItemSlotIcon> IconWidgetClass;
	
	//Hierarchy
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidgetOptional,AllowPrivateAccess="true"), Category = "UI")
	UTextBlock* ItemQuantity;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	USizeBox* IconBox;
};
