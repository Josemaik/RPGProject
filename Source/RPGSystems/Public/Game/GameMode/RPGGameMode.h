// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RPGGameMode.generated.h"

class UTypeOrderDataAsset;
class URarityDataAsset;
class UProjectileInfo;
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

	UProjectileInfo* GetProjectileInfo() const;

	URarityDataAsset* GetRarityDA() const;

	UTypeOrderDataAsset* GetTypeOrderDA() const;
private:
	UPROPERTY(EditDefaultsOnly, Category= "Default Values|Class Defaults")
	TObjectPtr<UCharacterClassInfo> ClassDefaults;

	UPROPERTY(EditDefaultsOnly, Category= "Default Values|Class Defaults")
	TObjectPtr<UProjectileInfo> ProjectileInfo;

	UPROPERTY(EditDefaultsOnly, Category= "Default Values|Class Defaults")
	TObjectPtr<URarityDataAsset> RarityColorInfo;

	UPROPERTY(EditDefaultsOnly, Category= "Default Values|Class Defaults")
	TObjectPtr<UTypeOrderDataAsset> TypeOrderDataAsset;
};
