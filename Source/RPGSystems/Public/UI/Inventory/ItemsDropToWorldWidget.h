// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemsDropToWorldWidget.generated.h"

struct FRPGInventoryEntry;
class UTextBlock;
class UImage;
class UBorder;

DECLARE_DELEGATE_OneParam(FOnItemDroppedEvent, const FRPGInventoryEntry&);
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemsDropToWorldWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnItemDroppedEvent OnItemDroppedPanelDelegate; 
private:
	virtual void NativeDestruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	void SetDropHighlight(bool bEnabled) const;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "UI")
	UBorder* DropItemToWorldContainer;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "UI")
	UImage* DropItemImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category = "UI")
	UTextBlock* DropItemText;
};
