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
	void AddItem(UItemSlotWidget* Item);
	void RemoveItem(const int64 ItemID);
	UItemSlotWidget* ContainsItem(const int64 ItemID);
	void UpdateItem(const FRPGInventoryEntry& Entry);
	
	void ClearPanel();
	int32 GetMaxColums() const { return MaxColumns; }
	void ResetCategory(FGameplayTag CurrentCategoryTag);
	
	FGameplayTag CurrentCategoryTag;

private:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "UI")
	UUniformGridPanel* ItemsPanel;
	
	//TMap<FGameplayTag, int32> CategoriesIndexMap;
	TMap<FGameplayTag, TArray<UItemSlotWidget*>> CategoryItemsMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category = "Data")
	int32 MaxColumns = 5;
};
