// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentSlot.generated.h"

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
private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess=true))
	UImage* EquipmentSlotImage;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= "Config",meta=(AllowPrivateAccess=true))
	FGameplayTag EquipmentTag;
};
