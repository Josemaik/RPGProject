// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTip.generated.h"

struct FMasterItemDefinition;
class UBorder;
class UTextBlock;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemToolTip : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetData(const FMasterItemDefinition& ItemDefinition);
private:
	UPROPERTY(EditAnywhere, meta=(BindWidget), Category = "Layout")
	UTextBlock* ItemNameText;

	UPROPERTY(EditAnywhere,meta=(BindWidget), Category = "Layout")
	UTextBlock* ItemDescriptionText;

	UPROPERTY(EditAnywhere,meta=(BindWidget), Category = "Layout")
	UBorder* RarityBorder;

	UPROPERTY(EditAnywhere,meta=(BindWidget), Category = "Layout")
	UTextBlock* RarityText;

	UPROPERTY(EditAnywhere,meta=(BindWidget), Category = "Layout")
	UTextBlock* LevelText;

	UPROPERTY(EditAnywhere,meta=(BindWidget), Category = "Layout")
	UTextBlock* WeightText;
	
	UPROPERTY(EditAnywhere,meta=(BindWidget), Category = "Layout")
	UTextBlock* DurabilityText;

	UPROPERTY(EditAnywhere,meta=(BindWidget), Category = "Layout")
	UTextBlock* PriceText;
};
