// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSortTypes.h"
#include "Blueprint/UserWidget.h"
#include "SortOptionEntryWidget.generated.h"

class UButton;
class UTextBlock;
class UCheckBox;
class USortOptionEntryWidget;

DECLARE_DELEGATE_TwoParams(FOnOptionButtonClicked, USortOptionEntryWidget*, EItemSortType);
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API USortOptionEntryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnOptionButtonClicked OnButtonClicked;
	void SetOption(EItemSortType SortType);
	void UnCheck() const;
private:
	UPROPERTY()
	EItemSortType Option;
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnOptionButtonClicked();
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="Layout")
	UButton* OptionButton;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="Layout")
	TObjectPtr<UCheckBox> CheckBox;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="Layout")
	TObjectPtr<UTextBlock> OptionText;
};
