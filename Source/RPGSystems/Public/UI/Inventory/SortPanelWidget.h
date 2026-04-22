// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSortTypes.h"
#include "Blueprint/UserWidget.h"
#include "SortPanelWidget.generated.h"

class UButton;
class USortOptionEntryWidget;
class UVerticalBox;

DECLARE_DELEGATE(FOnCloseButtonClicked);
DECLARE_DELEGATE_OneParam(FOnOptionChanged, EItemSortType);
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API USortPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnCloseButtonClicked OnCloseButtonClicked;
	FOnOptionChanged OnOptionChanged;
private:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnCloseButtonClick();

	UFUNCTION()
	void OnPressedOptionEntry(USortOptionEntryWidget* OptionEntry, EItemSortType Option);

	UPROPERTY()
	TObjectPtr<USortOptionEntryWidget> LastPressedOptionEntryWidget;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="UI|Widget")
	UVerticalBox* OptionsEntryVerticalBox;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="UI|Widget")
	UButton* CloseButton;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TSubclassOf<USortOptionEntryWidget> SortOptionEntryClass;
};
