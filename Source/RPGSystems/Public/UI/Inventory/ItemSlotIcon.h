// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotIcon.generated.h"

enum ESlotSizeCategories : uint8;
class USizeBox;
class UImage;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemSlotIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetIcon(TObjectPtr<UTexture2D> IconTexture, ESlotSizeCategories Size);
private:

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FVector2f IconSizeUniqueSlot = FVector2f(120.f,120.f);

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FVector2f IconSizeTwoSlots = FVector2f(120.f,200.f);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess="true"), Category = "UI")
	UImage* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess="true"), Category = "UI")
	USizeBox* SizeBox;
};
