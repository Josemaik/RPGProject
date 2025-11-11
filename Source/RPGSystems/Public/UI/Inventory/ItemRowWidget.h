// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "InventorySection/InventoryComponent.h"
#include "InventorySection/ItemTypes.h"
#include "ItemRowWidget.generated.h"

struct FRPGInventoryEntry;
struct FMasterItemDefinition;
class UTextBlock;
class UButton;


DECLARE_DELEGATE_OneParam(FOnUseButtomClicked, const FRPGInventoryEntry&);
DECLARE_DELEGATE_OneParam(FOnItemRowClicked, const FRPGInventoryEntry&);
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
	
	void SetActionText(const FGameplayTag& GameplayTag);
	void SetItemNameText(FText Text);
	void SetQuantityText(int32 Quantity);

	void SetInventoryEntry(const FRPGInventoryEntry& InItemDefinition);
	
	UPROPERTY(BlueprintReadOnly)
	FRPGInventoryEntry ItemEntry;
	
protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
private:

	//Callbacks
	UFUNCTION()
	void OnClickedUseButtom();
	
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
};
