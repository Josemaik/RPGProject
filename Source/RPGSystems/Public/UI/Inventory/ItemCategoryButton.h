// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "ItemCategoryButton.generated.h"


class UImage;
class UButton;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCategorySelected,FGameplayTag/*CategoryTag*/);

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemCategoryButton : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnCategorySelected OnCategorySelected;

	void Init(FGameplayTag Category,FText CategoryText, UTexture2D* IconTexture);

	void DeSelect();
	void Select();
private:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnActionButtonHovered();
	UFUNCTION()
	void OnActionButtonUnhovered();
	UFUNCTION()
	void OnActionButtonClicked();

	bool Selected = false;

	UPROPERTY(EditDefaultsOnly, Category = "Style")
	FLinearColor HoverColor;

	UPROPERTY(EditDefaultsOnly, Category = "Style")
	FLinearColor UnhoverColor;

	UPROPERTY(EditDefaultsOnly, Category = "Style")
	FLinearColor SelectedColor;

	UPROPERTY(EditInstanceOnly, meta=(AllowPrivateAccess=true),Category= "TagInfo")
	FGameplayTag CategoryTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess="true"), Category = "UI")
	UButton* ActionButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess="true"), Category = "UI")
	UImage* Icon;
};
