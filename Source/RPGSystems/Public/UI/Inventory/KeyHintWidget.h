// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeyHintWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UKeyHintWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetKeyHint(FText InKey, FText InAction) const;
private:
	UPROPERTY(EditDefaultsOnly,meta = (BindWidget), Category = "Key Hint")
	UTextBlock* KeyHintTextBlock;

	UPROPERTY(EditDefaultsOnly,meta = (BindWidget), Category = "Key Hint")
	UTextBlock* ActionTextBlock;	
};
