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
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemRowWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetActionText(const FGameplayTag& GameplayTag);
	void SetItemNameText(FText Text);
	void SetQuantityText(int32 Quantity);

	virtual void NativeConstruct() override;

	void SetInventoryEntry(const FRPGInventoryEntry& InItemDefinition);
	
	FOnUseButtomClicked	OnUseButtomClickedDelegate;
	
	FRPGInventoryEntry ItemEntry;
private:

	//Callbacks
	UFUNCTION()
	void OnClickedUseButtom();
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag GameplayTagConsumable;

	UPROPERTY(EditDefaultsOnly)
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
