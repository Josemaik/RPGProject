// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "CategoryButton.generated.h"


class UImage;
class UButton;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCategorySelected,FGameplayTag/*CategoryTag*/);

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UCategoryButton : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnCategorySelected OnCategorySelected;
private:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnActionButtonClicked();

	UPROPERTY(EditInstanceOnly, meta=(AllowPrivateAccess=true),Category= "TagInfo")
	FGameplayTag CategoryTag;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess="true"), Category = "UI")
	UTextBlock* ItemCategoryName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess="true"), Category = "UI")
	UButton* ActionButtom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess="true"), Category = "UI")
	UImage* Icon;
};
