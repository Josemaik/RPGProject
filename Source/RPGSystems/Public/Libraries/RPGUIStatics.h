// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventorySection/ItemTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGUIStatics.generated.h"

/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API URPGUIStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	static FLinearColor GetColorForRarity(const UObject* WorldContext, EItemRarity Rarity);

	UFUNCTION(BlueprintCallable, Category = "UI")
	static FText GetTextForRarity(const UObject* WorldContext, EItemRarity Rarity);
};
