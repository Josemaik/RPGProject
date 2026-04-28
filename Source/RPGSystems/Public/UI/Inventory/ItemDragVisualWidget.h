// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDragVisualWidget.generated.h"

UENUM(BlueprintType)
enum class EDragOverResult : uint8
{
	Drop    UMETA(DisplayName = "Drop"), 
	Swap    UMETA(DisplayName = "Swap"),    
	Invalid UMETA(DisplayName = "Invalid")
};

enum ESlotSizeCategories : uint8;
class USizeBox;
class UImage;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemDragVisualWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetIcon(TObjectPtr<UTexture2D> IconTexture, ESlotSizeCategories Size);
	void EnableDragOverResultIcon(EDragOverResult Result);
	void DisableDragOverResultIcon();
private:

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FVector2f IconSizeUniqueSlot = FVector2f(120.f,120.f);

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FVector2f IconSizeTwoSlots = FVector2f(120.f,200.f);

	UPROPERTY(EditDefaultsOnly, Category="Style")
	TObjectPtr<UTexture2D> DragOverResultDropTexture;

	UPROPERTY(EditDefaultsOnly, Category="Style")
	TObjectPtr<UTexture2D> DragOverResultSwapTexture;

	UPROPERTY(EditDefaultsOnly, Category="Style")
	TObjectPtr<UTexture2D> DragOverResultInvalidTexture;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess="true"), Category = "UI")
	UImage* DragIcon;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget,AllowPrivateAccess="true"), Category = "UI")
	UImage* DragOverResultIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess="true"), Category = "UI")
	USizeBox* SizeBox;
};
