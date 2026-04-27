// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentSlot.generated.h"

enum class EItemRarity : uint8;
class USizeBox;
struct FRPGInventoryEntry;
DECLARE_DELEGATE_OneParam(FOnEquipItem, const FRPGInventoryEntry&)

class UImage;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UEquipmentSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	FOnEquipItem OnEquipItem;
	
	void EquipItemSlot(const FRPGInventoryEntry& Entry,UTexture2D* Texture,EItemRarity Rarity);
private:
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(BindWidget,AllowPrivateAccess=true))
	UImage* EquipmentSlotImage;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(BindWidget,AllowPrivateAccess=true))
	UImage* RarityBackground;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Config",meta=(AllowPrivateAccess=true))
	FGameplayTag EquipmentTag;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Config",meta=(BindWidget,ExposeOnSpawn,AllowPrivateAccess=true))
	USizeBox* SizeBox;

	bool bIsEmpty = true;
};
