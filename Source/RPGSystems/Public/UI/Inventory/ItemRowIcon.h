// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemRowIcon.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API UItemRowIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetIcon(TSoftObjectPtr<UTexture2D>& Texture);
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess="true"), Category = "UI")
	UImage* Icon;
};
