// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumSections.h"
#include "Blueprint/UserWidget.h"
#include "SectionSwitcherWidget.generated.h"

class UWidgetSwitcher;
class UInputContextWidget;
class UTopBarWidget;
class UInventoryWidget;
class ARPGPlayerController;
class UInventoryWidgetController;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API USectionSwitcherWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayerControllerRef(ARPGPlayerController* PlayerController);
	void ChangeSection(EUISections Section);
	
	//Widget Controllers
	UInventoryWidgetController* GetInventoryWidgetController();
	void InitializeTopBarWidget();
	EUISections GetSection() const { return CurrentSection; }
private:
	virtual void NativeConstruct() override;
	
	void HandleSectionNavigation(int32 Direction);
	FString EnumToString(EUISections Section);
	
	//References
	UPROPERTY()
	ARPGPlayerController* PlayerControllerRef;

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetControllerRef;
	
	UPROPERTY()
	UInventoryWidget* InventoryWidgetRef;

	EUISections CurrentSection;
	TArray<EUISections> SectionsCarousel;
	int32 CurrentSectionIndex;
	
	//Layout
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTopBarWidget> TopBarWidget;

	UPROPERTY(EditDefaultsOnly,Category = "UI",meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> SectionWidgetSwitcher;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInputContextWidget> InputContextWidget;
	
	
	//Classes
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UInventoryWidgetController> InventoryWidgetControllerClass;
};


