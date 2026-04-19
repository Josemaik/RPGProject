// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumSections.h"
#include "Blueprint/UserWidget.h"
#include "InputContextWidget.generated.h"


class UURPGUIInputContextData;
class UKeyHintWidget;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UInputContextWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetKeyHints(EUISections Section) const;
private:
	
	UPROPERTY(EditDefaultsOnly, Category = "UI" , meta=(BindWidget))
	UHorizontalBox* HorizontalBox;
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UKeyHintWidget> KeyHintClass;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Data")
	TObjectPtr<UURPGUIInputContextData> InputContext;
};
