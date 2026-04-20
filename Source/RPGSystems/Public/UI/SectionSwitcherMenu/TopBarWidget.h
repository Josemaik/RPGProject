// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TopBarWidget.generated.h"

class UTopBarViewModel;
class UProgressBar;
class UButton;
class UHorizontalBox;
class UImage;
class UInventoryWidgetController;
class UTextBlock;

// Delegate que escucha SectionSwitcher
DECLARE_DELEGATE_OneParam(FOnSectionChanged, int32 Direction); // +1 o -1
/**
 * Pure View — displays data from UTopBarViewModel via MVVM bindings.
 */
UCLASS()
class RPGSYSTEMS_API UTopBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitCarousel(int32 NumSections);
	//void UpdateExperience(int Level, float CurrentExperience,float RequiredExperience) const;
	void SetActiveSection(int32 Index);

	//void SetSectionData(FString PreviousSection, FString CurrentSection, FString NextSection) const;
	
	//void SetInventoryWidgetController(UInventoryWidgetController* WidgetController);

	FOnSectionChanged OnSectionChanged;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTopBarViewModel> TopBarViewModel;
private:
	virtual void NativeConstruct() override;
	//void HandleInventoryWeightChanged(float InInventoryWeight);
	
	UFUNCTION()
	void OnPrevClicked();
	
	UFUNCTION()
	void OnNextClicked();
	
	///References
	// UPROPERTY()
	// UInventoryWidgetController* InventoryWidgetControllerRef;

	//Carousel state
	UPROPERTY(EditDefaultsOnly)
	FLinearColor ActiveColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor InactiveColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.f);
	
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess = true), Category = "UI")
	TObjectPtr<UTexture2D> CircleTexture;
	
	TArray<UImage*> CarouselImages;
	int32 CurrentIndex = 0;

	
	bool bIsInventoryWeightGreaterThanAvailable = false;
	
	////////////////////////////
	//Layout - bound widgets

	//Weight
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = "UI")
	UTextBlock* CurrentInventoryWeightText;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = "UI")
	UTextBlock* MaxInventoryWeightText;

	//Carousel
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = "UI")
	UTextBlock* CurrentSectionText;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = "UI")
	UTextBlock* PreviousSectionText;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = "UI")
	UTextBlock* NextSectionText;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category = "UI")
	UHorizontalBox* HorizontalBoxCarousel;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtomPreviousCategory;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtomNextCategory;

	///Level / XP
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget),Category = "UI")
	UTextBlock* LevelText;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget),Category = "UI")
	UProgressBar* ExperienceBar;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget),Category = "UI")
	UTextBlock* CurrentLevelExperienceText;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget),Category = "UI")
	UTextBlock* CurrentLevelMaxExperienceText;
};


