// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RPGGameMode.generated.h"

class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class RPGSYSTEMS_API ARPGGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	UCharacterClassInfo* GetCharacterClassDefaultInfo() const;

private:
	UPROPERTY(EditDefaultsOnly, Category= "Default Values|Class Defaults")
	TObjectPtr<UCharacterClassInfo> ClassDefaults;
};
