// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "InventorySection/ItemTypes.h"
#include "ItemRowWidget.generated.h"

class USizeBox;
class UItemRowIcon;
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
class RPGSYSTEMS_API UItemRowWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnUseButtomClicked	OnUseButtomClickedDelegate;
	FOnItemRowClicked OnItemRowClickedDelegate;
	FOnItemDroppedEvent OnItemDroppedEventDelegate;
	
	void SetActionText(const FGameplayTag& GameplayTag);
	void SetItemNameText(FText Text);
	void SetQuantityText(int32 Quantity);
	void SetIcon();
	
	void SetInventoryEntry(const FRPGInventoryEntry& InItemDefinition,TSoftObjectPtr<UTexture2D> Icon);
	
	UPROPERTY(BlueprintReadOnly)
	FRPGInventoryEntry ItemEntry;
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
private:

	//Callbacks
	UFUNCTION()
	void OnClickedUseButtom();

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> SoftIconTexture;

	UPROPERTY()
	UItemRowIcon* IconWidgetReference;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	TSubclassOf<UItemRowIcon> IconWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FGameplayTag GameplayTagConsumable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FGameplayTag GameplayTagEquipment;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UTextBlock* ItemName;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UTextBlock* ItemQuantity;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UTextBlock* ActionText;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UButton* ActionButtom;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	USizeBox* IconBox;
};
