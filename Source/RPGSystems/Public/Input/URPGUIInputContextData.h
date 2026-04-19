// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/SectionSwitcherMenu/EnumSections.h"
#include "URPGUIInputContextData.generated.h"


USTRUCT(BlueprintType)
struct FKeyActions
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FString Key;

	UPROPERTY(EditDefaultsOnly)
	FString Action;
};

USTRUCT(BlueprintType)
struct FSectionKeyHints
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	EUISections Section;

	UPROPERTY(EditDefaultsOnly)
	TArray<FKeyActions> KeyActions;
};

/**
 * 
 */
UCLASS(BlueprintType)
class RPGSYSTEMS_API UURPGUIInputContextData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FSectionKeyHints> SectionKeyHints; 
};
