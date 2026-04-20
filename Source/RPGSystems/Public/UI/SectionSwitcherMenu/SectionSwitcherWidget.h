// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumSections.h"
#include "Blueprint/UserWidget.h"
#include "Game/PlayerState/RPGPlayerState.h"
#include "SectionSwitcherWidget.generated.h"

class ARPGPlayerState;
class UMissionsWidget;
class UCharacterBuildWidget;
class UWorldMapWidget;
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
	void SetPlayerControllerRef(ARPGPlayerController* PlayerController,ARPGPlayerState* PlayerState);
	void OpenSection(EUISections Section);
	void ChangeSection(EUISections Section);
	
	//Widget Controllers
	UInventoryWidgetController* GetInventoryWidgetController();
	void InitializeTopBarWidget();
private:
	virtual void NativeConstruct() override;
	
	void HandleSectionNavigation(int32 Direction);
	FString EnumToString(EUISections Section);
	
	//References
	UPROPERTY()
	ARPGPlayerController* PlayerControllerRef;

	UPROPERTY()
	ARPGPlayerState* CachedPlayerState;

	UPROPERTY()
	TObjectPtr<UInventoryWidgetController> InventoryWidgetControllerRef;

	//Widgets Sections Refs
	UPROPERTY()
	UInventoryWidget* InventoryWidgetRef;
	
	UPROPERTY()
	UWorldMapWidget* WorldMapWidgetRef;
	
	UPROPERTY()
	UCharacterBuildWidget* CharacterBuildWidgetRef;

	UPROPERTY()
	UMissionsWidget* MissionsWidgetRef;

	//Carousel 
	TArray<EUISections> SectionsCarousel;
	EUISections CurrentSection;
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

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UWorldMapWidget> WorldMapWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UCharacterBuildWidget> CharacterBuildWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Widgets")
	TSubclassOf<UMissionsWidget> MissionsdWidgetClass;
};


