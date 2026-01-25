// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "ItemsPanelWidget.generated.h"

struct FRPGInventoryEntry;
class UItemSlotWidget;
class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemsPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddItemToGrid(UItemSlotWidget* Item,const int32 Index);
	void RemoveItem(const int64 ItemID);
	UItemSlotWidget* ContainsItemSlot(const int64 ItemID);
	
	void UpdateItemSlot(const FRPGInventoryEntry& Entry);
	FGameplayTag GetItemCategory(FGameplayTag ItemTag);
	UItemSlotWidget* AddItemSlot(const FRPGInventoryEntry& Entry,TSoftObjectPtr<UTexture2D> Icon);
	
	void ClearPanel();
	int32 GetMaxColums() const { return MaxColumns; }
	void AddEmptySlots(FGameplayTag InCurrentCategoryTag);
	void ResetCategory(FGameplayTag CurrentCategoryTag);
	
	FGameplayTag CurrentCategoryTag;

private:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UUniformGridPanel* ItemsPanel;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;
	
	//TMap<FGameplayTag, int32> CategoriesIndexMap;
	TMap<FGameplayTag, TArray<UItemSlotWidget*>> CategoryItemsMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 MaxColumns = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 NUM_INITIAL_EMPTY_SLOTS = 35;
};
